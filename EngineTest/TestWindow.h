#pragma once
#include "Test.h"
#include "..\Platform\Platform.h"

using namespace primal;

platform::window _windows[4];

class engine_test :public test
{
public:
	bool initialize() override
	{
		for (u32 i{ 0 }; i < _countof(_windows); ++i)
			_windows[i] = platform::create_window();
		return true;
	}

	void run() override
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	void shutdown() override
	{
		for (u32 i{ 0 }; i < _countof(_windows); ++i)
		{
			platform::remove_window(_windows[i].get_id());
		}
	}
};