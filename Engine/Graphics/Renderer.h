#pragma once
#include "CommonHeaders.h"
#include "..\Platform\Window.h"

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
}