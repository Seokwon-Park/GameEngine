#pragma once
#include "CommonHeaders.h"

namespace primal::utl
{
	// std vectro와 기본적인 기능은 유사
	// remove, clear, destruct할때 호출할 수 있는 elements destructor를 template args에 설정할 수 있다.
	template<typename T, bool destruct = true>
	class vector
	{
	public:
		// default constructor. doesn't allocate memory.
		vector() = default;

		// constructor resizes the vector and initializes 'count' items.
		// sizeof (unsigned 64) == sizeof(size_t) in x64 platform
		constexpr explicit vector(u64 count)
		{
			resize(count);
		}

		// constructor resizes the vector and initializes 'count' items using 'value'.
		constexpr explicit vector(u64 count, const T& value)
		{
			resize(count, value);
		}

		// Copy-constructor. Constructs by copying another vector. The items
		// in the copied vector must be copyable
		constexpr vector(const vector& o)
		{
			*this = o;
		}
		
		// Move-constructor. Constructs by copying another vector. 
		// The original vector will be empty after move.
		constexpr vector(const vector&& o)
			:_capacity{ o._capacity }, _size{ o._size }, _data{o._data}
		{
			o.reset();
		}

		// Copy-assignment operator. Clears this vector and copies items
		// from another vector. The items must be copyable.
		constexpr vector& operator=(const vector& o)
		{
			assert(this != std::addressof(o));
			if (this != std::addressof(o))
			{
				clear();
				reserve(o._size);
				for (auto& item : o)
				{
					emplace_back(item);
				}
				assert(_size == o._size);
			}

			return *this;
		}

		// Move-assignment operator. Frees all resources in this vector and
		// moves the other vector into this one.
		constexpr vector& operator=(const vector&& o)
		{
			assert(this != std::addressof(o));
			if (this != std::addressof(o))
			{
				destroy();
				move(o);
			}

			return *this;
		}

		~vector()
		{
			destroy();
		}

		// Clears the vector and destruct items as specified in template argument.
		constexpr void clear()
		{
			if constexpr (destruct)
			{
				destruct_range(0, _size);
			}
			_size = 0;
		}



	private:
		constexpr void move(vector& o)
		{
			_capacity = o._capacity;
			_size = o._size;
			_data = o._data;
			o.reset();
		}

		constexpr void reset()
		{
			_capacity = 0;
			_size = 0;
			_data = nullptr;
		}

		constexpr void 	destruct_range(u64 first, u64 last)
		{
			assert(destruct);
			assert(first <= _size && last <= _size && first <= last);
			if (_data)
			{
				for (; first != last; ++first)
				{
					_data[first].~T();
				}
			}
		}

		constexpr void destroy()
		{
			assert([&] {return _capacity ? _data != nullptr : _data == nullptr; }());
			clear();
			_capacity = 0;
			if (_data) free(_data);
			_data = nullptr;
		}


		u64 _capacity{ 0 };
		u64 _size{ 0 };
		T* _data{ nullptr };

	};
}