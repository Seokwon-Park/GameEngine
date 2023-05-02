#pragma once

#include "CommonHeaders.h"

namespace primal::platform 
{
	DEFINE_TYPED_ID(window_id);
	class window
	{
	public:
		constexpr explicit window(window_id id) : _id{ id } {}
		constexpr window() : _id{ id::invalid_id } {}
		constexpr window_id get_id() const { return _id; }
		constexpr bool is_valid() const { return id::is_valid(_id); }

	private:
		window_id _id;
	};
}