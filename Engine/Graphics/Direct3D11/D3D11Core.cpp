#include "D3D11Core.h"

namespace primal::graphics::d3d11::core
{
	namespace
	{
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	} // anonymous namespace

	bool initialize()
	{
		// determine which adapter (i.e. graphics card) to use
		// determine what is the maximum feature level that is supporter
		// create a ID3D12Device ( this is a virtual adapter).
		return true;
	}

	void shutdown()
	{

	}
}