#include "Test.h"

#pragma comment(lib, "engine.lib")

#if TEST_ENTITY_COMPONENTS
#include "TestEntityComponents.h"
#elif TEST_WINDOW
#include "TestWindow.h"
#elif TEST_RENDERER
#include "TestRenderer.h"
#else
#error One of the tests need to be enabled
#endif

#ifdef _WIN64
#include <Windows.h>
#include <filesystem>

// TODO: duplicate
std::filesystem::path set_current_directory_to_executable_path()
{
	// set the working directory to the executable path
	wchar_t path[MAX_PATH];//윈도우 최대경로 길이 260
	const uint32_t length{ GetModuleFileName(0, &path[0], MAX_PATH) };//실행중인 실행파일의 위치를 path에 저장
	if (!length || GetLastError() == ERROR_INSUFFICIENT_BUFFER) return {}; // 경로가 너무 길어도 false
	std::filesystem::path p{ path };//받아온 path를 저장
	std::filesystem::current_path(p.parent_path());
	return std::filesystem::current_path();
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#if _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	set_current_directory_to_executable_path();
	engine_test test{};
	if (test.initialize())
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
			 
			test.run();
		}
	}
	test.shutdown();
	return 0;
}
#else
int main()
{
#if _DEBUG
	//메모리 누수 감지
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	engine_test test{};

	if (test.initialize())
	{
		test.run();
	}

	test.shutdown();	
}
#endif