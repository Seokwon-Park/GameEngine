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

		// need to delete const?
		// Move-constructor. Constructs by copying another vector. 
		// The original vector will be empty after move.
		constexpr vector(vector&& o)
			:_capacity{ o._capacity }, _size{ o._size }, _data{ o._data }
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
		constexpr vector& operator=(vector&& o)
		{
			assert(this != std::addressof(o));
			if (this != std::addressof(o))
			{
				destroy();
				move(o);
			}

			return *this;
		}

		// Destructs the vector and its items as specified in template argument
		~vector()
		{
			destroy();
		}

		// Inserts an item at the end of the vector by copying 'value'.
		constexpr void push_back(const T& value)
		{
			emplace_back(value);
		}

		// Inserts an item at the end of the vector by moving 'value'.
		constexpr void push_back(T&& value)
		{
			emplace_back(std::move(value));
		}

		// Copy- or move-constructs an item at the end of the vector.
		template<typename... params>
		constexpr decltype(auto) emplace_back(params&&... p)
		{
			if (_size == _capacity)
			{
				reserve(((_capacity + 1) * 3) >> 1); //reserve 50% more
			}
			assert(_size < _capacity);

			T* const item{ new (std::addressof(_data[_size])) T(std::forward<params>(p)...) };
			++_size;
			return *item;
		}
		// Resizes the vector and initializes new items with their default value.
		constexpr void resize(u64 new_size)
		{
			static_assert(std::is_default_constructible<T>::value,
				"Type must be copy-constructible.");

			if (new_size > _size)
			{
				reserve(new_size);
				while (_size < new_size)
				{
					emplace_back();
				}
			}
			else if (new_size < _size)
			{
				if constexpr (destruct)
				{
					destruct_range(new_size, _size);
				}

				_size = new_size;
			}

			// Do Nothing if new_size == _size.
			assert(new_size == _size);
		}

		// Resizes the vector and initializes new items by copying 'value'
		constexpr void resize(u64 new_size, const T& value)
		{
			static_assert(std::is_copy_constructible<T>::value,
				"Type must be copy-constructible.");

			if (new_size > _size)
			{
				reserve(new_size);
				while (_size < new_size)
				{
					emplace_back(value);
				}
			}
			else if (new_size < _size)
			{
				if constexpr (destruct)
				{
					destruct_range(new_size, _size);
				}

				_size = new_size;
			}

			// Do Nothing if new_size == _size.
			assert(new_size == _size);
		}

		// Allocates memory to contain the specified number of items.
		constexpr void reserve(u64 new_capacity)
		{
			if (new_capacity > _capacity)
			{
				// NOTE: relloc() will automatically copy the data in the buffer
				//		 if a new region of memory is allocated.
				void* new_buffer{ realloc(_data, new_capacity * sizeof(T)) };
				assert(new_buffer);
				if (new_buffer)
				{
					_data = static_cast<T*>(new_buffer);
					_capacity = new_capacity;
				}
			}
		}

		// Removes the item at specified index
		constexpr T* const erase(u64 index)
		{
			assert(_data && index < size);
			return erase(std::addressof(_data[index]));
		}

		// Removes the item at specified location.
		constexpr T* const erase(T* const item)
		{
			assert(_data && item >= std::addressof(_data[0]) &&
				item < std::addressof(_data[_size]));
			if constexpr (destruct) item->~T();
			--_size;
			if (item < std::addressof(_data[_size]))
			{
				memcpy(item, item + 1, (std::addressof(_data[_size]) - item) * sizeof(T));
			}

			return item;
		}

		// Same as erase() but faster because it just copies the last item.
		constexpr T* const erase_unordered(u64 index)
		{
			assert(_data && index < _size);
			return erase_unordered(std::addressof(_data[index]));
		}

		// Same as erase() but faster because it just copies the last item.
		constexpr T* const erase_unordered(T* const item)
		{
			assert(_data && item >= std::addressof(_data[0]) &&
				item < std::addressof(_data[_size]));
			if constexpr (destruct) item->~T();
			--_size;
			if (item < std::addressof(_data[_size]))
			{
				memcpy(item, std::addressof(_data[_size]), sizeof(T));
			}

			return item;
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

		constexpr void swap(vector& o)
		{
			if (this != std::addressof(o))
			{
				auto temp(std::move(o));
				o = std::move(*this);
				*this = std::move(temp);
			}
		}

		// Pointer to the start of data. Might be null.
		[[nodiscard]] constexpr T* data()
		{
			return _data;
		}

		// Pointer to the start of data. Might be null.
		[[nodiscard]] constexpr T* const data() const
		{
			return _data;
		}

		// Returns true if vector is empty
		[[nodiscard]] constexpr bool empty() const
		{
			return _size == 0;
		}

		// Returns the number of items in the vector.
		[[nodiscard]] constexpr u64 size() const
		{
			return _size;
		}

		// Returns the current capacity of the vector.
		[[nodiscard]] constexpr u64 capacity()const
		{
			return _capacity;
		}

		// Indexing operator. returns a reference to the item at specified index.
		[[nodiscard]] constexpr T& operator[](u64 index)
		{
			assert(_data && index < _size);
			return _data[index];
		}

		// Indexing operator. returns a constant reference to the item at specified index.
		[[nodiscard]] constexpr const T& operator[](u64 index) const
		{
			assert(_data && index < _size);
			return _data[index];
		}

		// Returns a reference to the first item. Will Fault the application if called
		// when the vector is empty
		[[nodiscard]] constexpr T& front()
		{
			assert(_data && _size);
			return _data[0];
		}

		// Returns a constant reference to the first item. Will Fault the application if called
		// when the vector is empty
		[[nodiscard]] constexpr const T& front() const
		{
			assert(_data && _size);
			return _data[0];
		}

		// Returns a reference to the last item. Will Fault the application if called
		// when the vector is empty
		[[nodiscard]] constexpr T& back()
		{
			assert(_data && _size);
			return _data[_size - 1];
		}

		// Returns a constant reference to the last item. Will Fault the application if called
		// when the vector is empty
		[[nodiscard]] constexpr const T& back() const
		{
			assert(_data && _size);
			return _data[_size - 1];
		}

		// Returns a pointer to the first item. Returns null when vector is empty
		[[nodiscard]] constexpr T* begin()
		{
			return std::addressof(_data[0]);
		}

		// Returns a constant pointer to the first item. Returns null when vector is empty
		[[nodiscard]] constexpr const T* begin() const
		{
			return std::addressof(_data[0]);
		}

		// Returns a pointer to the last item. Returns null when vector is empty
		[[nodiscard]] constexpr T* end()
		{
			assert(!(_data == nullptr&& _size>0));
			return std::addressof(_data[_size]);
		}

		// Returns a constant pointer to the last item. Returns null when vector is empty
		[[nodiscard]] constexpr const T* end() const
		{
			assert(!(_data == nullptr && _size > 0));
			return std::addressof(_data[_size]);
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