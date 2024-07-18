#pragma once

#include <functional>

namespace concrete {

	template<class T, class binary_operation>
	struct group_traits {};

	template<class T> requires ::std::is_arithmetic_v<T>
	struct group_traits<T, ::std::plus<>> {
		using value_type = T;
		using binary_operation = ::std::plus<value_type>;

		static constexpr value_type identity{0};
		using inverse_operation = ::std::negate<value_type>;
		using binary_inverse_operation = ::std::minus<value_type>;
	};

	template<class T> requires ::std::is_arithmetic_v<T>
	struct group_traits<T, ::std::multiplies<>> {
		using value_type = T;
		using binary_operation = ::std::multiplies<value_type>;

		static constexpr value_type identity{1};
		using binary_inverse_operation = ::std::divides<value_type>;

		struct inverse_operation {
			constexpr value_type operator()(const value_type& x) const noexcept {
				return binary_inverse_operation{}(identity, x);
			}
		};
	};

}