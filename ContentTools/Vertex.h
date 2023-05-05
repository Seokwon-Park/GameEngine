#pragma once
#include "ToolsCommon.h"

namespace primal::tools{
	struct Vertex
	{
		math::Vector4 tangent{};
		math::Vector3 position{};
		math::Vector3 normal{};
		math::Vector2 uv{};
	};
}