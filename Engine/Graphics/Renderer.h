#pragma once
#include "CommonHeaders.h"
#include "Platform/Window.h"
#include "EngineAPI/Camera.h"

namespace primal::graphics 
{

	DEFINE_TYPED_ID(surface_id);

	class surface {
	public:
		constexpr explicit surface(surface_id id) : _id{ id } {}
		constexpr surface() = default;
		constexpr surface_id get_id() const { return _id; }
		constexpr bool is_valid() const { return id::is_valid(_id); }

		void resize(u32 width, u32 height) const;
		u32 width() const;
		u32 height() const;
		void render() const;

	private:
		surface_id _id{ id::invalid_id };
	};

	struct render_surface
	{
		platform::window window{};
		surface surface{};
	};

	struct camera_parameter
	{
		enum parameter :u32
		{
			up_vector,
			field_of_view,
			aspect_ratio,
			view_width,
			view_height,
			near_z,
			far_z,
			view,
			projection,
			inverse_projection,
			view_projection,
			inverse_view_projection,
			type,
			entity_id,

			count
		};
	};

	struct camera_init_info
	{
		id::id_type entity_id{ id::invalid_id };
		camera::type type{};
		math::v3 up;
		union
		{
			f32 field_of_view;
			f32 view_width;
		};
		union
		{
			f32 aspect_ratio;
			f32 view_height;
		};
		f32 near_z;
		f32 far_z;
	};

	struct perspective_camera_init_info : public camera_init_info
	{
		explicit perspective_camera_init_info(id::id_type id)
		{
			assert(id::is_valid(id));
			entity_id = id;
			type = camera::perspective;
			up = { 0.f, 1.f, 0.f };
			field_of_view = 0.25f;
			aspect_ratio = 16.f / 10.f;
			near_z = 0.001f;
			far_z = 10000.f;

		}
	};

	struct orthographic_camera_init_info : public camera_init_info
	{
		explicit orthographic_camera_init_info(id::id_type id)
		{
			assert(id::is_valid(id));
			entity_id = id;
			type = camera::orthographic;
			up = { 0.f, 1.f, 0.f };
			view_width = 1920;
			view_height = 1080;
			near_z = 0.001f;
			far_z = 10000.f;
		}
	};

	enum class graphics_platform :u32
	{
		direct3d12 = 0,
	};

	bool initialize(graphics_platform platform);
	void shutdown();
	void render();

	// 실행가능한 경로에서 컴파일된 엔진 셰이더의 상대 경로를 구합니다. 
	// 이 경로는 현재 사용되고 있는 graphics api를 위한 것.
	const char* get_engine_shaders_path();


	// 실행가능한 경로에서 컴파일된 엔진 셰이더의 상대 경로를 구합니다. + 특정 플랫폼에 대해
	// 이 경로는 현재 사용되고 있는 graphics api를 위한 것.
	const char* get_engine_shaders_path(graphics_platform platform);

	surface create_surface(platform::window window);
	void remove_surface(surface_id id);

	camera create_camera(camera_init_info info);
	void remove_camera(camera_id id);

	id::id_type add_submesh(const u8* data);
	void remove_submesh(id::id_type id);
}