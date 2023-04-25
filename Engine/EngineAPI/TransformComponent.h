#pragma once

#include "..\Components\ComponentsCommon.h"

namespace primal::transform {

	DEFINE_TYPED_ID(transform_id);
	class component final
	{
	public:
		constexpr explicit component(transform_id id) : _id{ id } {}
		constexpr component() :_id{ id::invalid_id } {}
		constexpr transform_id get_id() const { return _id; }
		constexpr bool is_valid() const { return id::is_valid(_id); }

		math::Vector4 rotation() const;
		math::Vector3 position() const;
		math::Vector3 scale() const;

		//math::v4 rotation() const;
		//math::v3 position() const;
		//math::v3 scale() const;
	private:
		transform_id _id;
	};
}

