#include "netvar_manager.h"
#include <windows.h>
#include <SDKDDKVer.h>
#include <string.h>
#include <vadefs.h>
#include <stdio.h>
#include <xstring>
#include <Psapi.h>
#include <thread>
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <time.h>
#include <winuser.h>
#include <random>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>
#include <thread>
#include <array>
#include <assert.h>
#include <deque>
#include <intrin.h>
#include <atomic>
#include <mutex>
#include <stdint.h>
#include <stdlib.h>
#include <process.h>
#include <Wincon.h>
#include <cstdint>
#include <chrono>
#include <Shlobj.h>
#include <future>
#include <Lmcons.h>
#include <tchar.h>

#include "../sdk/recv.h"
#include "fnv.h"
#include "../interfaces/interfaces.h"
#include "../sdk/client_class.h"
#include "../sdk/i_client_entity.h"
#include "../hooks/hooks.h"

uintptr_t netvar_tree::get_net_var(const uint32_t table,
	const uint32_t prop) {
	static netvar_table_map map = {};
	if (map.empty())
		initialize_props(map);

	if (map.find(table) == map.end())
		return 0;

	netvar_key_value_map& table_map = map.at(table);
	if (table_map.find(prop) == table_map.end())
		return 0;

	return table_map.at(prop);
}

void netvar_tree::add_props_for_table(netvar_table_map& table_map, const uint32_t table_name_hash, const std::string& table_name, recv_table* table, const bool dump_vars, std::map< std::string, std::map< uintptr_t, std::string > >& var_dump, const size_t child_offset) {
	for (auto i = 0; i < table->props_count; ++i) {
		auto& prop = table->props[i];

		if (prop.data_table && prop.elements_count > 0) {
			if (std::string(prop.prop_name).substr(0, 1) == std::string("0"))
				continue;

			add_props_for_table(table_map, table_name_hash, table_name, prop.data_table, dump_vars, var_dump, prop.offset + child_offset);
		}

		auto name = std::string(prop.prop_name);

		if (name.substr(0, 1) != "m")
			continue;

		const auto name_hash = fnv_hash(prop.prop_name);
		const auto offset = uintptr_t(prop.offset) + child_offset;

		if (strstr(prop.prop_name, "m_nSequence") && table_name_hash == fnv_hash("DT_BaseViewModel"))
			hooks::sequence_hk = new recv_prop_hook(&table->props[i], hooks::sequence_hook);

		table_map[table_name_hash][name_hash] = offset;

		if (dump_vars)
			var_dump[table_name][offset] = prop.prop_name;
	}
}

void netvar_tree::initialize_props(netvar_table_map& table_map) {
	const auto dump_vars = true; 

	std::map< std::string, std::map< uintptr_t, std::string > > var_dump;
	for (auto client_class = interfaces::client->get_client_classes();
		client_class;
		client_class = client_class->next_ptr) {
		const auto table = reinterpret_cast<recv_table*>(client_class->recvtable_ptr);
		const auto table_name = table->table_name;
		const auto table_name_hash = fnv_hash(table_name);

		if (table == nullptr)
			continue;

		add_props_for_table(table_map, table_name_hash, table_name, table, dump_vars, var_dump);
	}
}

std::unique_ptr<netvar_tree> netvar_manager;