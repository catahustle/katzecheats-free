#include <Windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>

#include "utilities/utilities.h"
#include "interfaces/interfaces.h"
#include "utilities/render_manager.h"
#include "hooks/hooks.h"
#include "utilities/netvar_manager.h"
#include "features/skins/skinchanger.h"
#include "menu/settings.h"

#include "utilities/imgui/imgui.h"
#include "utilities/imgui/imgui_impl_dx9.h"
#include "utilities/imgui/imgui_impl_win32.h"

VOID WINAPI setup_debug_console() {
	AllocConsole();

	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	SetConsoleTitleA("[purpura sdk] console: ");
}

DWORD WINAPI dll_setup(HINSTANCE module_handle) {
	srand(time(nullptr));

#ifdef _DEBUG
	setup_debug_console();

	global_utils::console_log("impact debug console loaded.");
#endif

	c_config::init();
	
	interfaces::init();

	netvar_manager = std::make_unique < netvar_tree >();
	netvar_manager->get_offset("DT_BaseViewModel", "m_nSequence"); // meme

	features::skins->initialize(); // thx to namaszo

	hooks::init();

#ifdef _DEBUG
	while (true) {
		using namespace std::literals::chrono_literals;
		std::this_thread::sleep_for(1s);

		if (hooks::g_unload) {
			fclose((FILE*)stdin);
			fclose((FILE*)stdout);

			HWND hw_ConsoleHwnd = GetConsoleWindow();

			FreeConsole();
			PostMessageW(hw_ConsoleHwnd, WM_CLOSE, 0, 0);

			hooks::restore();

			delete hooks::sequence_hk;

			Sleep(1000);

			FreeLibraryAndExitThread(module_handle, 1);
		}
	}
#endif

	return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD  reason_for_call, LPVOID reserved) {
	switch (reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(module);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)dll_setup, module, NULL, NULL);
		break;
	}

	return TRUE;
}