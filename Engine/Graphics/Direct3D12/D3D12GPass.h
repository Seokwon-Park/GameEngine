#pragma once

#include "D3D12CommonHeaders.h"

namespace primal::graphics::d3d12::gpass {

	bool initialize();
	void shutdown();

	// gpass에 어떤 것을 렌더링 하던 매 프레임마다 이것을 호출 
	void set_size(math::u32v2 size)
}
