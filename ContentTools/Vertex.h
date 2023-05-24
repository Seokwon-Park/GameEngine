#pragma once
#include "ToolsCommon.h"

namespace primal::tools {
	struct Vertex
	{
		math::v4 tangent{};
		math::v4 joint_weights{};
		math::u32v4 joint_indices{ u32_invalid_id, u32_invalid_id, u32_invalid_id, u32_invalid_id };
		math::v3 position{};
		math::v3 normal{};
		math::v2 uv{};
		u8 red{}, green{}, blue{};
		u8 pad;
	};
}