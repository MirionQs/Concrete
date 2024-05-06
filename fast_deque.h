#pragma once

#include <cstddef>

namespace concrete {

	template<class T>
	struct fast_deque {
		T* _begin;
		T* _first;
		T* _last;

		fast_deque(::std::size_t capacity) noexcept {
			_begin = _first = _last = new T[capacity];
		}

		~fast_deque() noexcept {
			delete[] _begin;
		}

		bool empty() const noexcept {
			return _first == _last;
		}

		void clear() noexcept {
			_first = _last = _begin;
		}

		T* begin() const noexcept {
			return _first;
		}

		T* end() const noexcept {
			return _last;
		}

		T& operator[](::std::size_t index) const noexcept {
			return _first[index];
		}

		T& front() const noexcept {
			return *_first;
		}

		T& back() const noexcept {
			return _last[-1];
		}

		void push_front(const T& value) noexcept {
			*--_first = value;
		}

		void pop_front() noexcept {
			++_first;
		}

		void push_back(const T& value) noexcept {
			*_last++ = value;
		}

		void pop_back() noexcept {
			--_last;
		}
	};

}