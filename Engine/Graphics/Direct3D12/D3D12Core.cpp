#include "D3D12Core.h"

namespace primal::graphics::d3d12::core
{
	namespace
	{
		ID3D12Device8* device;
	} // anonymous namespace

	bool initialize()
	{
		// determine which adapter (i.e. graphics card) to use
		// determine what is the maximum feature level that is supporter
		// create a ID3D12Device ( this is a virtual adapter).
	}

	void shutdown()
	{

	}
}