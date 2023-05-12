/* Things todo to create a game project:
*
	1) Generate an MSVC solution/project
	2) Add files that contain the script
	3) Set include and library directories
	4) Set force include file (GameEntity.h>
	5) SEt c++ language version and calling convension
*/
#include "CommonHeaders.h"
#include <filesystem>

#ifdef _WIN64
//자주 사용하지 않는 Win32 API의 일부를 제외하는 매크로
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif // !WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <crtdbg.h>
namespace {
	// TODO: we might want to have an IO utility header/library and move this function in ther
	std::filesystem::path set_current_directory_to_executable_path()
	{
		// set the working directory to the executable path
		wchar_t path[MAX_PATH]{};//윈도우 최대경로 길이 260
		const u32 length{ GetModuleFileName(0, &path[0], MAX_PATH) };//실행중인 실행파일의 위치를 path에 저장
		if (!length || GetLastError() == ERROR_INSUFFICIENT_BUFFER) return {}; // 경로가 너무 길어도 false
		std::filesystem::path p{ path };//받아온 path를 저장
		std::filesystem::current_path(p.parent_path());
		return std::filesystem::current_path();
	}
}

#ifndef USE_WITH_EDITOR

extern bool engine_initialize();
extern void engine_update();
extern void engine_shutdown();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	set_current_directory_to_executable_path();

	if (engine_initialize())
	{
		MSG msg{};
		bool is_running{ true };
		//main loop 
		//engine update function		
		while (is_running)
		{
			//처리할 메시지가 남지 않을 때까지 메시지 큐에서 메시지 읽기, 제거, 발송
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				is_running &= (msg.message != WM_QUIT);
			}

			engine_update();
		}			
	}
	engine_shutdown();
	return 0;
}

#endif // !USE_WITH_EDITOR
#endif // _WIN64
