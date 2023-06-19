#include "D3D12Light.h"
#include "Shaders/SharedTypes.h"
#include "EngineAPI/GameEntity.h"

namespace primal::graphics::d3d12::light
{
	namespace
	{
		struct light_owner
		{
			game_entity::entity_id entity_id{ id::invalid_id };
			u32 data_index;
			graphics::light::type type;
			bool is_enabled;
		};

		class light_set
		{
		public:
			constexpr graphics::light add(const light_init_info& info)
			{
				if (info.type == graphics::light::directional)
				{
					u32 index{ u32_invalid_id };
					// Find an available slot in the array if any.
					for (u32 i{ 0 }; i < _non_cullable_owners.size(); ++i)
					{
						if (!id::is_valid(_non_cullable_owners[i]))
						{
							index = i;
							break;
						}
					}

					if (index == u32_invalid_id)
					{
						index = (u32)_non_cullable_owners.size();
						_non_cullable_owners.emplace_back();
						_non_cullable_lights.emplace_back();
					}

					
					hlsl::DirectionalLightParameters& params{ _non_cullable_lights[index] };
					params.Color = info.color;
					params.Intensity = info.intensity;

					light_owner owner{ game_entity::entity_id{info.entity_id}, index, info.type, info.is_enabled };
					const light_id id{ _owners.add(owner) };
					_non_cullable_owners[index] = id;

					return graphics::light{ id, info.light_set_key };
				}
				else
				{
					// TODO: cullable lights
				}
			}
			
			constexpr void remove(light_id id)
			{
				enable(id, false);

				const light_owner& owner{ _owners[id] };

				if (owner.type == graphics::light::directional)
				{
					_non_cullable_owners[owner.data_index] = light_id{ id::invalid_id };
				}
				else
				{
					// TODO: cullable lights
				}

				_owners.remove(id);
			}

			constexpr void enable(light_id id, bool is_enabled) 
			{
				_owners[id].is_enabled = is_enabled;

				if (_owners[id].type == graphics::light::directional)
				{
					return;
				}

				// TODO: cullable lights
			}

			constexpr void intensity(light_id id, f32 intensity)
			{
				if (intensity < 0.f) intensity = 0.f;

				const light_owner& owner{ _owners[id] };
				const u32 index{ owner.data_index };

				if (owner.type == graphics::light::directional)
				{
					assert(index < _non_cullable_lights.size());
					_non_cullable_lights[index].Intensity = intensity;
				}
				else
				{
					// TODO: cullable lights
				}
			}

			constexpr void color(light_id id, math::v3 color)
			{
				assert(color.x <= 1.f && color.y <= 1.f && color.z <= 1.f);
				assert(color.x >= 0.f && color.y >= 0.f && color.z >= 0.f);

				const light_owner& owner{ _owners[id] };
				const u32 index{ owner.data_index };

				if (owner.type == graphics::light::directional)
				{
					assert(index < _non_cullable_lights.size());
					_non_cullable_lights[index].Color = color;
				}
				else
				{
					// TODO: cullable lights
				}
			}

			constexpr bool is_enabled(light_id id) const
			{
				return _owners[id].is_enabled;
			}

			constexpr f32 intensity(light_id id)
			{
				const light_owner& owner{ _owners[id] };
				const u32 index{ owner.data_index };

				if (owner.type == graphics::light::directional)
				{
					assert(index < _non_cullable_lights.size());
					return _non_cullable_lights[index].Intensity;
				}
				else
				{
					// TODO: cullable lights
				}
			}

			constexpr math::v3 color(light_id id)
			{
				const light_owner& owner{ _owners[id] };
				const u32 index{ owner.data_index };

				if (owner.type == graphics::light::directional)
				{
					assert(index < _non_cullable_lights.size());
					return _non_cullable_lights[index].Color;
				}
				else
				{
					// TODO: cullable lights
				}
			}

			constexpr graphics::light::type type(light_id id) const 
			{
				return _owners[id].type;
			}

			constexpr id::id_type entity_id(light_id id) const
			{
				return _owners[id].entity_id;
			}

			// REturn the number of enabled directional lights
			constexpr u32 non_cullable_light_count() const
			{
				u32 count{ 0 };
				for (const auto& id : _non_cullable_owners)
				{
					if (id::is_valid(id) && _owners[id].is_enabled) ++count;
				}

				return count;
			}

			constexpr void non_cullable_lights(hlsl::DirectionalLightParameters *const lights, [[maybe_unused]] u32 buffer_size)
			{
				assert(buffer_size == math::align_size_up<D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT>(non_cullable_light_count() * sizeof(hlsl::DirectionalLightParameters)));
				const u32 count{ (u32)_non_cullable_owners.size() };
				u32 index{ 0 };
				for (u32 i{ 0 }; i < count; ++i)
				{
					if (!id::is_valid(_non_cullable_owners[i])) continue;

					const light_owner& owner{ _owners[_non_cullable_owners[i]] };
					if (owner.is_enabled)
					{
						assert(_owners[_non_cullable_owners[i]].data_index == i);
						lights[index] = _non_cullable_lights[i];
						++index;
					}
				}
			}

		private:
			// NOTE: these are not tightly packed
			utl::free_list<light_owner>						_owners;
			utl::vector<hlsl::DirectionalLightParameters>	_non_cullable_lights;
			utl::vector<light_id>							_non_cullable_owners;
		};

		std::unordered_map<u64, light_set> light_sets;

		constexpr void dummy_set(light_set&, light_id, const void* const, u32)
		{

		}

		using set_function = void(*)(light_set&, light_id, const void* const, u32);
		using get_function = void(*)(light_set&, light_id, void* const, u32);

		constexpr set_function set_functions[]
		{
			dummy_set,
		};

		static_assert(_countof(set_functions) == graphics::camera_parameter::count);

		constexpr get_function get_functions[]
		{
			get_type,
			get_entity_id
		};

		static_assert(_countof(get_functions) == graphics::camera_parameter::count);

	} // anonymous namespace
	graphics::light create(light_init_info info)
	{
		return{};
	}
	void remove(light_id id, u64 light_set_key)
	{}
	void set_parameter(light_id id, u64 light_set_key, light_parameter::parameter parameter, const void* const data, u32 data_size)
	{}
	void get_parameter(light_id id, u64 light_set_key, light_parameter::parameter parameter, void* const data, u32 data_size)
	{}
}

