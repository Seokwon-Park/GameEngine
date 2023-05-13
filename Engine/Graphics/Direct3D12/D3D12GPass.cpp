#include "D3D12GPass.h"
#include "D3D12Core.h"

namespace primal::graphics::d3d12::gpass 
{
	namespace
	{
		constexpr DXGI_FORMAT main_buffer_format{ DXGI_FORMAT_R16G16B16A16_FLOAT };
		constexpr DXGI_FORMAT depth_buffer_format{ DXGI_FORMAT_D32_FLOAT };
		constexpr math::u32v2 initial_dimensions{ 100,100 };

		d3d12_render_texture gpass_main_buffer{};
		d3d12_depth_buffer gpass_depth_buffer{};
		math::u32v2 dimensions{ initial_dimensions };

#if _DEBUG
		constexpr f32 clear_value[4]{ 0.5f, 0.5f, 0.5f, 1.f };
#else
		constexpr f32 clear_value[4]{ };
#endif // _DEBUG


		bool create_buffers(math::u32v2 size)
		{
			assert(size.x && size.y);
			gpass_main_buffer.release();
			gpass_depth_buffer.resource();

			D3D12_RESOURCE_DESC desc{};
			desc.Alignment = 0; // note: 0 is the smae as 64kb (or 4mb for msaa)
			desc.DepthOrArraySize = 1;
			desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			desc.Format = main_buffer_format;
			desc.Height = size.y;
			desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			desc.MipLevels = 0; // make space for all mip levels
			desc.SampleDesc = { 1,0 };
			desc.Width = size.x;

			//Create the main buffer
			{
				d3d12_texture_init_info info{};
				info.desc = &desc;
				info.initial_state = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				info.clear_value.Format = desc.Format;
				memcpy(&info.clear_value.Color, &clear_value[0], sizeof(clear_value));
				gpass_main_buffer = d3d12_render_texture{ info };					
			}

			desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			desc.Format = depth_buffer_format;
			desc.MipLevels = 1;

			//Create the depth buffer
			{
				d3d12_texture_init_info info{};
				info.desc = &desc;
				info.initial_state = D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				info.clear_value.Format = desc.Format;
				info.clear_value.DepthStencil.Depth = 0.f;
				info.clear_value.DepthStencil.Stencil = 0;

				gpass_depth_buffer = d3d12_depth_buffer{ info };
			}

			NAME_D3D12_OBJECT(gpass_main_buffer.resource(), L"GPass Main Buffer");
			NAME_D3D12_OBJECT(gpass_depth_buffer.resource(), L"GPass Depth Buffer");

			return gpass_main_buffer.resource() && gpass_depth_buffer.resource();
		}
	} //anonymouse namespace

	bool initialize()
	{
		return create_buffers(initial_dimensions);
	}

	void shutdown()
	{
		gpass_main_buffer.release();
		gpass_depth_buffer.release();
		dimensions = initial_dimensions;
	}
}

