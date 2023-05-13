#pragma once

#include "D3D12CommonHeaders.h"

namespace primal::graphics::d3d12::gpass {

	bool initialize();
	void shutdown();

	// gpass�� � ���� ������ �ϴ� �� �����Ӹ��� �̰��� ȣ�� 
	void set_size(math::u32v2 size)
}
