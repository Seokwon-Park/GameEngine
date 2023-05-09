#include "CommonHeaders.h"
#include "D3D12Interface.h"
#include "D3D12Core.h"
#include "Graphics\GraphicsPlatformInterface.h"

namespace primal::graphics::d3d12
{
	void get_platform_interface(platform_interface& phi)
	{
		phi.initialize = core::initialize;
		phi.shutdown = core::shutdown;
	}
}

