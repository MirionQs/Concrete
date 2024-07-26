#pragma once

#include "operator.h"

namespace concrete {

	template <class T, class Op = ::concrete::add<T>>
	class fenwick_tree : private ::std::vector<T> {
		using base = ::std::vector<T>;

		static constexpr ::concrete::assignment_operator_t<Op> _op{};

		T& _at(::std::size_t index) noexcept {
			return base::operator[](index);
		}

		template<class Fn>
		void _apply(::std::size_t index, Fn function) {
			::std::size_t size{this->size()};
			while (index < size) {
				function(_at(index));
				index |= index + 1;
			}
		}

	public:
		explicit fenwick_tree(::std::size_t size) noexcept : base(size) {}

		template <class input_iterator>
		explicit fenwick_tree(input_iterator first, input_iterator last) noexcept : base(first, last) {
			::std::size_t size{this->size()};
			for (::std::size_t i{0}; i != size; ++i) {
				::std::size_t j{i | i + 1};
				if (j < size) {
					_op(_at(j), _at(i));
				}
			}
		}

		void apply(::std::size_t index, const T& value) noexcept {
			_apply(index, [&](T& x) { _op(x, value); });
		}

		void apply_inverse(::std::size_t index, const T& value) noexcept {
			static_assert(::concrete::is_group_v<Op>, "Op should be invertible.");

			static constexpr auto invOp{::concrete::inverse_assignment_operator_t<Op>{}};
			_apply(index, [&](T& x) { invOp(x, value); });
		}

		T operator[](::std::size_t index) noexcept {
			T res{};
			::std::size_t size{this->size()};
			while (index < size) {
				_op(res, _at(index));
				index &= index + 1;
				--index;
			}
			return res;
		}
	};

	template <class T, class Op = ::concrete::add<T>>
	class fenwick_tree_diff : private fenwick_tree<T, Op> {
		static_assert(::concrete::is_group_v<Op>, "Op should be invertible.");

		using base = fenwick_tree<T, Op>;

		static constexpr auto _invOp{::concrete::inverse_assignment_operator_t<Op>{}};

		template <class input_iterator>
		static base _construct(input_iterator first, input_iterator last) {
			::std::vector<T> diff(first, last);
			for (::std::size_t i{diff.size() - 1}; i != 0; --i) {
				diff[i] = _invOp(diff[i], diff[i - 1]);
			}
			return base{diff.begin(), diff.end()};
		}

	public:
		using base::operator[];

		explicit fenwick_tree_diff(::std::size_t size) noexcept : base{size} {}

		template <class input_iterator>
		explicit fenwick_tree_diff(input_iterator first, input_iterator last) noexcept : base{::std::move(_construct(first, last))} {}

		void apply(::std::size_t first, ::std::size_t last, const T& value) noexcept {
			base::apply(first, value);
			base::apply_inverse(last, value);
		}
	};
}