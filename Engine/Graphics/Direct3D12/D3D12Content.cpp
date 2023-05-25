#include "D3D12Content.h"
#include "D3D12Core.h"
#include "Utilities/IOStream.h"
#include "Content/ContentToEngine.h"

namespace primal::graphics::d3d12::content
{

	namespace
	{
		struct position_view
		{
			D3D12_VERTEX_BUFFER_VIEW position_buffer_view{};
			D3D12_INDEX_BUFFER_VIEW index_buffer_view{};
		};

		struct element_view
		{
			D3D12_VERTEX_BUFFER_VIEW element_buffer_view{};
			u32 element_type{};
			D3D_PRIMITIVE_TOPOLOGY primitive_topology;
		};

		utl::free_list<ID3D12Resource*> submesh_buffers{};
		utl::free_list<position_view> position_views{};
		utl::free_list<element_view> element_views{};
		std::mutex submesh_mutex{};

		D3D_PRIMITIVE_TOPOLOGY get_d3d_primitive_topology(primal::content::primitive_topology::type type)
		{
			using namespace primal::content;
			assert(type < primitive_topology::count);
			switch (type)
			{
			case primal::content::primitive_topology::point_list:return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case primal::content::primitive_topology::line_list:return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case primal::content::primitive_topology::line_strip:return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case primal::content::primitive_topology::triangle_list:return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case primal::content::primitive_topology::triangle_strip:return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			}

			return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	} // anonymous namespace
	namespace submesh
	{
		// NOTE: 'data'가 포함할 것으로 기대되는것
		// struct {
		//		u32 element_size, u32 vertex_count,
		//		u32 index_count, u32 elements_type, u32 primitive topology
		//		u8 positions[sizeof(f32) * 3 * vertex_count],		// sizeof(positions) must be a multiple of 4bytes. Pad if needed.
		//		u8 elements[sizeof(element_size) * vertex_count],	// sizeof(elements) must be a multiple of 4bytes. Pad if needed.
		//		u8 indices[index_size * index_count],
		// } submeshes[submesh_count];
		// 
		// Remarks:
		// - Advances the data pointer
		// - Position and element buffers should be padded to be a multiple of 4 bytes in length.
		//	 This 4 bytes is defined as D3D12_RAW_UAV_SRT_BYTE_ALIGNMENT.
		id::id_type add(const u8*& data)
		{
			utl::blob_stream_reader blob{ (const u8*)data };

			const u32 element_size{ blob.read<u32>() };
			const u32 vertex_count{ blob.read<u32>() };
			const u32 index_count{ blob.read<u32>() };
			const u32 elements_type{ blob.read<u32>() };
			const u32 primitive_topology{ blob.read<u32>() };
			const u32 index_size{ (vertex_count < (1 << 16)) ? sizeof(u16) : sizeof(u32) };

			// NOTE: element size may be 0, for position-only vertex formats.
			const u32 position_buffer_size{ sizeof(math::v3) * vertex_count };
			const u32 element_buffer_size{ element_size * vertex_count };
			const u32 index_buffer_size{ index_size * vertex_count };

			constexpr u32 alignment{ D3D12_STANDARD_MAXIMUM_ELEMENT_ALIGNMENT_BYTE_MULTIPLE };
			const u32 aligned_position_buffer_size{ (u32)math::align_size_up<alignment>(position_buffer_size) };
			const u32 aligned_element_buffer_size{ (u32)math::align_size_up<alignment>(element_buffer_size) };
			const u32 total_buffer_size{ aligned_position_buffer_size + aligned_element_buffer_size + index_buffer_size };

			ID3D12Resource* resource{ d3dx::create_buffer(blob.position(), total_buffer_size) };

			blob.skip(total_buffer_size);
			data = blob.position();

			position_view position_view{};
			position_view.position_buffer_view.BufferLocation = resource->GetGPUVirtualAddress();
			position_view.position_buffer_view.SizeInBytes = position_buffer_size;
			position_view.position_buffer_view.StrideInBytes = sizeof(math::v3);

			position_view.index_buffer_view.BufferLocation = resource->GetGPUVirtualAddress() + aligned_position_buffer_size + aligned_element_buffer_size;
			position_view.index_buffer_view.Format = (index_size == sizeof(u16)) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			position_view.index_buffer_view.SizeInBytes = position_buffer_size;

			element_view element_view{};
			if (element_size)
			{
				element_view.element_buffer_view.BufferLocation = resource->GetGPUVirtualAddress() + aligned_position_buffer_size;
				element_view.element_buffer_view.SizeInBytes = element_buffer_size;
				element_view.element_buffer_view.StrideInBytes = element_size;
			}

			element_view.element_type = elements_type;
			element_view.primitive_topology = get_d3d_primitive_topology((primal::content::primitive_topology::type)primitive_topology);

			std::lock_guard lock{ submesh_mutex };
			submesh_buffers.add(resource);
			position_views.add(position_view);
			return element_views.add(element_view);
		}

		void remove(id::id_type id)
		{
			std::lock_guard lock{ submesh_mutex };
			position_views.remove(id);
			element_views.remove(id);

			core::deferred_release(submesh_buffers[id]);
			submesh_buffers.remove(id);
		}
	} // namespace submesh
}