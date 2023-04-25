#pragma once
#include "CommonHeader.h"


namespace primal::id
{
	using id_type = u32;

	constexpr u32 generation_bits{ 8 };
	constexpr u32 index_bits{ sizeof(id_type) * 8 - generation_bits };
	constexpr id_type index_mask{ (id_type{1} << index_bits) - 1 }; // 24비트 범위를 전부 1로
	constexpr id_type generation_mask{ (id_type{1} << generation_bits) - 1 }; // 8비트 범위를 전부 1로
	//&연산을 통해 마스킹 가능
	constexpr id_type id_mask{ id_type{-1} };
	//모든 유형의 id_type(u16, u32, u64)에 대해 유효하지 않은 id

	using generation_type = std::conditional_t<generation_bits <= 16, std::conditional_t<generation_bits <= 8, u8, u16>, u32 >;
	static_assert(sizeof(generation_type) * 8 >= generation_bits);
	static_assert(sizeof(id_type) - sizeof(generation_type) > 0);

	inline bool is_valid(id_type id)//id_mask와 다르면?
		//ex) 0 != -1 true 정상적인 정수이면 true를 반환한다.
	{
		return id != id_mask;
	}

	inline id_type index(id_type id)//index 구하는 함수
	{
		return id & index_mask;
	}

	inline id_type generation(id_type id)//generation구하는 함수
	{
		return (id >> index_bits) & generation_mask;
	}

	inline id_type new_generation(id_type id)
	{
		const id_type generation{ id::generation(id) + 1 };
		assert(generation < 255);
		return index(id) | (generation << index_bits);
		//ex) id = 1001일 때, generation(id)으로 호출 >>(right shift) 해서 0001 + 1 하면 0001
		//index_bits만큼 다시 올리면 generation은 2000에 index(id)와 or 연산으로 2001

	}

#if _DEBUG
	namespace internal
	{
		struct id_base
		{
			constexpr explicit id_base(id_type id) : _id{ id } {}
			constexpr operator id_type() const {
				return _id;
			}
		private:
			id_type _id;
		};
	}
#define DEFINE_TYPED_ID(name)										\
		struct name final : id :: internal :: id_base				\
		{															\
			constexpr explicit name(id::id_type id)					\
				: id_base{id} {}									\
			constexpr name() : id_base{ id::id_mask }{ }			\
		};
#else
#define DEFINE_TYPED_ID(name) using name = id::id_type;
#endif

}
