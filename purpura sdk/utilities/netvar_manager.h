#pragma once
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

class netvar_tree {
public:
	using netvar_key_value_map = std::unordered_map< uint32_t, uintptr_t >;
	using netvar_table_map = std::unordered_map< uint32_t, netvar_key_value_map >;

	netvar_tree() {}

	uintptr_t get_offset(const char* table, const char* prop) {
		return get_net_var(fnv_hash(table), fnv_hash(prop));
	}

	uintptr_t get_net_var(const uint32_t table, const uint32_t prop);
	void add_props_for_table(netvar_table_map& table_map, const uint32_t table_name_hash, const std::string& table_name, recv_table* table, const bool dump_vars, std::map< std::string, std::map< uintptr_t, std::string > >& var_dump, const size_t child_offset = 0);
	void initialize_props(netvar_table_map& table_map);
};

extern std::unique_ptr<netvar_tree> netvar_manager;

#define NETVAR(table, prop, func_name, type) \
	type& func_name( ) { \
	  if(!this) MessageBoxA(0, prop, "sorry, game is going to crash :(", MB_ICONERROR); \
      static uintptr_t offset = 0; \
      if(!offset) \
      { offset = netvar_manager->get_net_var(fnv_hash( table ), fnv_hash( prop ) ); } \
	  \
      return *reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
    }