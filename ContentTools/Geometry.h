#pragma once
#include "ToolsCommon.h"

namespace primal::tools {

	struct mesh
	{
		utl::vector<math::Vector3> positions;
		utl::vector<math::Vector3> normals;
		utl::vector<math::Vector3> tangents;
		utl::vector<utl::vector<math::Vector2>> uv_sets;

		utl::vector<u32> raw_indices;

		// Intermediate data

		// Output data
	};

	struct lod_group
	{
		std::string name;
		std::vector<mesh> meshes;
	};

	struct scene
	{
		std::string name;
		utl::vector<lod_group> lod_groups;
	};

	struct geometry_import_settings
	{
		f32 smoothing_angle;
		u8 calculate_normals;
		u8 calculate_tangents;
		u8 reverse_handedness;
		u8 import_embeded_textures;
		u8 import_animations;
	};

	struct scene_data
	{
		u8* buffer;
		u32 buffer_size;
		geometry_import_settings settings;
	};
}