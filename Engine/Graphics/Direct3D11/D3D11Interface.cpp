#include "CommonHeaders.h"
#include "D3D11Interface.h"
#include "D3D11Core.h"
#include "Graphics\GraphicsPlatformInterface.h"

namespace primal::graphics::d3d11
{
	void get_platform_interface(platform_interface& pi)
	{
		pi.initialize = core::initialize;
		pi.shutdown = core::shutdown;
	}
}