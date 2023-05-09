#include "Renderer.h"
#include "GraphicsPlatformInterface.h"

namespace primal::graphics
{
	namespace
	{
		platform_interface gfx{};

		bool set_platform_interface(graphics_platform platform)
		{
			switch (platform)
			{
			case primal::graphics::graphics_platform::direct3d11:
				d3d11::get_platform_interface(gfx);
				break;
			case primal::graphics::graphics_platform::direct3d12:
				d3d12::get_platform_interface(gfx);
				break;
			default:
				break;
			}
		}
	}// anonymous namespace

	bool initialize(graphics_platform platform)
	{
		return set_platform_interface(platform);
	}

	void shutdown()
	{
		gfx.shutdown();
	}
}