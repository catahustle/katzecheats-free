#pragma once

#include <vector>
#include <string>

#include "../../sdk/recv.h"
#include "../../sdk/entities.h"

namespace features
{
	struct kit
	{
		int id;
		std::string name;

		bool operator<(const kit& other)
		{
			return name < other.name;
		}
	};

	class skinchanger {
	public:
		void initialize();
		void apply();
		void fix_sequence(c_recv_proxy_data*, c_base_view_model*);

		std::vector<kit> skin_kits;
		std::vector<kit> glove_kits;
	};

	extern skinchanger* skins;
}