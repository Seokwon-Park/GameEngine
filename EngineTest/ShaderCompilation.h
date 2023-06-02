#pragma once

#include "Graphics/Direct3D12/D3D12Shaders.h"


struct shader_file_info
{
	const char* file_name;
	const char* function;
	primal::graphics::d3d12::shaders::shader_type::type type;
};

bool compile_shaders();