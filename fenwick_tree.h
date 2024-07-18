#pragma once

#include "group_traits.h"

namespace concrete {

	// point update and range query
	template <class T, class binary_operator = ::std::plus<>>
	class fenwick_tree_pr : private ::std::vector<T> {
		using base = ::std::vector<T>;

		static constexpr binary_operator _op{};

		T& _at(::std::size_t index) noexcept {
			return base::operator[](index);
		}

	public:
		explicit fenwick_tree_pr(::std::size_t size) noexcept : base(size) {}

		template <::std::input_iterator iter>
		explicit fenwick_tree_pr(iter first, iter last) noexcept : base(first, last) {
			::std::size_t size{this->size()};
			for (::std::size_t i{0}; i != size; ++i) {
				::std::size_t j{i | i + 1};
				if (j < size) {
					_at(j) = _op(_at(j), _at(i));
				}
			}
		}

		void apply(::std::size_t index, const T& value) noexcept {
			::std::size_t size{this->size()};
			while (index < size) {
				_at(index) = _op(_at(index), value);
				index |= index + 1;
			}
		}

		T operator[](::std::size_t index) noexcept {
			T res{};
			::std::size_t size{this->size()};
			while (index < size) {
				res = _op(res, _at(index));
				index &= index + 1;
				--index;
			}
			return res;
		}
	};

	// range update and point query
	template <
		class T,
		class binary_operator = ::std::plus<>,
		class inverse_operator = ::concrete::group_traits<T, binary_operator>::inverse_operation,
		class binary_inverse_operator = ::concrete::group_traits<T, binary_operator>::binary_inverse_operation
	>
	class fenwick_tree_rp : private fenwick_tree_pr<T, binary_operator> {
		using base = fenwick_tree_pr<T, binary_operator>;

		static constexpr inverse_operator _inv{};
		static constexpr binary_inverse_operator _binInv{};

		template <::std::input_iterator iter>
		static base _construct(iter first, iter last) {
			::std::vector<T> diff(first, last);
			for (::std::size_t i{diff.size() - 1}; i != 0; --i) {
				diff[i] = _binInv(diff[i], diff[i - 1]);
			}
			return base{diff.begin(), diff.end()};
		}

	public:
		using base::operator[];

		explicit fenwick_tree_rp(::std::size_t size) noexcept : base{size} {}

		template <::std::input_iterator iter>
		explicit fenwick_tree_rp(iter first, iter last) noexcept : base{::std::move(_construct(first, last))} {}

		void apply(::std::size_t first, ::std::size_t last, const T& value) noexcept {
			base::apply(first, value);
			base::apply(last, _inv(value));
		}
	};
}