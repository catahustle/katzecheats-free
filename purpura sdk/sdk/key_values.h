#pragma once

#include <Windows.h>
//#include <Psapi.h>

#include "../utilities/utilities.h"

/*static std::uintptr_t findPattern(const wchar_t* module, const char* pattern, size_t offset = 0)
{
    static auto id = 0;
    ++id;

    if (MODULEINFO moduleInfo; GetModuleInformation(GetCurrentProcess(), GetModuleHandleW(module), &moduleInfo, sizeof(moduleInfo))) {
        auto start = static_cast<const char*>(moduleInfo.lpBaseOfDll);
        const auto end = start + moduleInfo.SizeOfImage;

        auto first = start;
        auto second = pattern;

        while (first < end && *second) {
            if (*first == *second || *second == '?') {
                ++first;
                ++second;
            }
            else {
                first = ++start;
                second = pattern;
            }
        }

        if (!*second)
            return reinterpret_cast<std::uintptr_t>(const_cast<char*>(start) + offset);
    }
    MessageBoxA(NULL, ("Failed to find pattern #" + std::to_string(id) + '!').c_str(), "Osiris", MB_OK | MB_ICONWARNING);
    return 0;
}

#define FIND_PATTERN(type, ...) \
reinterpret_cast<type>(findPattern(__VA_ARGS__))*/

class key_values
{
public:
	static key_values* get_key_values_from_string(const char* name, const char* value)
	{
        //using fn = key_values * (__stdcall*)(char*, char*, char**);
		//static fn key_values_from_string = global_utils::relativeToAbsolute<decltype(key_values_from_string)>(FIND_PATTERN(int*, L"client_panorama", "\xE8????\x83\xC4\x04\x89\x45\xD8", 1));
        static DWORD key_values_from_string = *(DWORD*)(global_utils::get_signature("client_panorama.dll", "E8 ? ? ? ? 83 C4 04 89 45 D8") + 1);
		key_values* Ret;

        __asm {
            push 0
            mov edx, value
            mov ecx, name
            call key_values_from_string
            add esp, 4
            mov Ret, eax
        }

		return Ret;
	}
};