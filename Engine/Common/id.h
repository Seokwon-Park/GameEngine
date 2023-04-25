#pragma once
#include "CommonHeaders.h"


namespace primal::id
{
	using id_type = u32;

	namespace internal {
		constexpr u32 generation_bits{ 8 };
		constexpr u32 index_bits{ sizeof(id_type) * 8 - generation_bits };
		constexpr id_type index_mask{ (id_type{1} << index_bits) - 1 }; // 24��Ʈ ������ ���� 1��
		constexpr id_type generation_mask{ (id_type{1} << generation_bits) - 1 }; // 8��Ʈ ������ ���� 1��
		//&������ ���� ����ŷ ����
	} // internal namespace 
	constexpr id_type invalid_id{ id_type(-1)};
	//��� ������ id_type(u16, u32, u64)�� ���� ��ȿ���� ���� id
	constexpr u32 min_deleted_elements{ 1024 };
	// �迭�� ��Ҹ� 1024 �������� �� ũ�Ⱑ �� ũ�� ��밡���� ���Կ� �ٽ� ���� �����Ѵ�.

	using generation_type = std::conditional_t<internal::generation_bits <= 16, std::conditional_t<internal::generation_bits <= 8, u8, u16>, u32 >;
	static_assert(sizeof(generation_type) * 8 >= internal::generation_bits);
	static_assert(sizeof(id_type) - sizeof(generation_type) > 0);

	constexpr bool is_valid(id_type id)//id_mask�� �ٸ���?
		//ex) 0 != -1 true �������� �����̸� true�� ��ȯ�Ѵ�.
	{
		return id != invalid_id;
	}

	constexpr id_type index(id_type id)//index ���ϴ� �Լ�
	{
		id_type index{ id & internal::index_mask };
		assert(index != internal::index_mask);
		return id & internal::index_mask;
	}

	constexpr id_type generation(id_type id)//generation���ϴ� �Լ�
	{
		return (id >> internal::index_bits) & internal::generation_mask;
	}

	constexpr id_type new_generation(id_type id)
	{
		const id_type generation{ id::generation(id) + 1 };
		assert(generation < ((u64)1 << internal::generation_bits) -1);
		return index(id) | (generation << internal::index_bits);
		//ex) id = 1001�� ��, generation(id)���� ȣ�� >>(right shift) �ؼ� 0001 + 1 �ϸ� 0001
		//index_bits��ŭ �ٽ� �ø��� generation�� 2000�� index(id)�� or �������� 2001

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
		struct name final : id::internal::id_base					\
		{															\
			constexpr explicit name(id::id_type id)					\
				: id_base{id} {}									\
			constexpr name() : id_base{ 0 }{ }						\
		};
#else
#define DEFINE_TYPED_ID(name) using name = id::id_type;
#endif

}