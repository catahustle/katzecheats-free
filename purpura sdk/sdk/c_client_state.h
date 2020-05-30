#pragma once

#include <Windows.h>

class c_client_state {
public:
	void force_full_update()
	{
		*reinterpret_cast<int*>((DWORD)this + 0x174) = -1;
	}
};