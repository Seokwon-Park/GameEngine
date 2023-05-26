#pragma once
#include "CommonHeaders.h"
#include "Renderer.h"

namespace primal::graphics
{
	struct platform_interface
	{
		//function pointer
		bool(*initialize)(void);
		void(*shutdown)(void);
		void(*render)(void);

		struct {
			surface(*create)(platform::window);
			void(*remove)(surface_id);
			void(*resize)(surface_id, u32, u32);
			u32(*width)(surface_id);
			u32(*height)(surface_id);
			void(*render)(surface_id);
		}surface;

		struct
		{
			id::id_type (*add_submesh)(const u8*&);
			void (*remove_submesh)(id::id_type);
		} resources;
		graphics_platform platform = (graphics_platform)-1;
	};
}
