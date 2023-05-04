#pragma once
#include "ToolsCommon.h"

namespace primal::tools{
	struct Vertex
	{
		Vector4 tangent{};
		Vector3 position{};
		Vector3 normal{};
		Vector2 uv{};
	};
}