#pragma once

#include <functional>

namespace concrete {

	template<class T = void>
	using add = ::std::plus<T>;

	template<class T = void>
	using subtract = ::std::minus<T>;

	template<class T = void>
	using multiply = ::std::multiplies<T>;

	template<class T = void>
	using divide = ::std::divides<T>;

	template<class T = void>
	using modulo = ::std::modulus<T>;

	template<class T = void>
	struct add_assign {
		constexpr T& operator()(T& x, const T& y) const noexcept {
			return x += y;
		}
	};

	template<>
	struct add_assign<void> {
		template<class T, class U>
		constexpr T& operator()(T& x, const U& y) const noexcept {
			return x += y;
		}
	};

	template<class T = void>
	struct subtract_assign {
		constexpr T& operator()(T& x, const T& y) const noexcept {
			return x -= y;
		}
	};

	template<>
	struct subtract_assign<void> {
		template<class T, class U>
		constexpr T& operator()(T& x, const U& y) const noexcept {
			return x -= y;
		}
	};

	template<class T = void>
	struct multiply_assign {
		constexpr T& operator()(T& x, const T& y) const noexcept {
			return x *= y;
		}
	};

	template<>
	struct multiply_assign<void> {
		template<class T, class U>
		constexpr T& operator()(T& x, const U& y) const noexcept {
			return x *= y;
		}
	};

	template<class T = void>
	struct divide_assign {
		constexpr T& operator()(T& x, const T& y) const noexcept {
			return x /= y;
		}
	};

	template<>
	struct divide_assign<void> {
		template<class T, class U>
		constexpr T& operator()(T& x, const U& y) const noexcept {
			return x /= y;
		}
	};

	template<class T = void>
	struct modulo_assign {
		constexpr T& operator()(T& x, const T& y) const noexcept {
			return x %= y;
		}
	};

	template<>
	struct modulo_assign<void> {
		template<class T, class U>
		constexpr T& operator()(T& x, const U& y) const noexcept {
			return x %= y;
		}
	};

	template<class Op>
	struct assignment_operator {
		struct type {
			template<class T, class U>
			constexpr T& operator()(T& x, const U& y) const noexcept {
				return x = Op{}(x, y);
			}
		};
	};

	template<class T>
	struct assignment_operator<::concrete::add<T>> {
		using type = ::concrete::add_assign<T>;
	};

	template<class T>
	struct assignment_operator<::concrete::subtract<T>> {
		using type = ::concrete::subtract_assign<T>;
	};

	template<class T>
	struct assignment_operator<::concrete::multiply<T>> {
		using type = ::concrete::multiply_assign<T>;
	};

	template<class T>
	struct assignment_operator<::concrete::divide<T>> {
		using type = ::concrete::divide_assign<T>;
	};

	template<class T>
	struct assignment_operator<::concrete::modulo<T>> {
		using type = ::concrete::modulo_assign<T>;
	};

	template<class Op>
	using assignment_operator_t = typename ::concrete::assignment_operator<Op>::type;

	struct magma_tag {};
	struct semigroup_tag : ::concrete::magma_tag {};
	struct monoid_tag : ::concrete::semigroup_tag {};
	struct group_tag : ::concrete::monoid_tag {};
	struct abelian_magma_tag : ::concrete::magma_tag {};
	struct abelian_semigroup_tag : ::concrete::semigroup_tag, ::concrete::abelian_magma_tag {};
	struct abelian_monoid_tag : ::concrete::monoid_tag, ::concrete::abelian_semigroup_tag {};
	struct abelian_group_tag : ::concrete::group_tag, ::concrete::abelian_monoid_tag {};

	template<class Op>
	struct algebraic_traits {};

	template<class T>
	struct algebraic_traits<::concrete::add<T>> {
		using value_type = T;
		using operator_type = ::concrete::add<value_type>;

		static constexpr bool associative{true};
		static constexpr value_type identity{0};
		using inverse_operator = ::concrete::subtract<value_type>;
		static constexpr bool abelian{true};

		using algebraic_structure_tag = ::concrete::abelian_group_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::subtract<T>> {
		using value_type = T;
		using operator_type = ::concrete::subtract<value_type>;

		static constexpr bool associative{false};
		static constexpr bool abelian{false};

		using algebraic_structure_tag = ::concrete::magma_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::multiply<T>> {
		using value_type = T;
		using operator_type = ::concrete::multiply<value_type>;

		static constexpr bool associative{true};
		static constexpr value_type identity{1};
		using inverse_operator = ::concrete::divide<value_type>;
		static constexpr bool abelian{true};

		using algebraic_structure_tag = ::concrete::abelian_group_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::divide<T>> {
		using value_type = T;
		using operator_type = ::concrete::divide<value_type>;

		static constexpr bool associative{false};
		static constexpr bool abelian{false};

		using algebraic_structure_tag = ::concrete::magma_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::modulo<T>> {
		using value_type = T;
		using operator_type = ::concrete::modulo<value_type>;

		static constexpr bool associative{false};
		static constexpr bool abelian{false};

		using algebraic_structure_tag = ::concrete::magma_tag;
	};

	template<class T, class Compare = ::std::less<T>>
	struct maximum {
		constexpr T operator()(const T& x, const T& y) const noexcept {
			return ::std::max<T, Compare>(x, y);
		}
	};

	template<class T, class Compare = ::std::less<T>>
	struct minimum {
		constexpr T operator()(const T& x, const T& y) const noexcept {
			return ::std::min<T, Compare>(x, y);
		}
	};

	template<class T, class Compare>
	struct algebraic_traits<::concrete::maximum<T, Compare>> {
		using value_type = T;
		using operator_type = ::concrete::maximum<value_type, Compare>;

		static constexpr bool associative{true};
		static constexpr bool abelian{true};

		using algebraic_structure_tag = ::concrete::abelian_semigroup_tag;
	};

	template<class T, class Compare>
	struct algebraic_traits<::concrete::minimum<T, Compare>> {
		using value_type = T;
		using operator_type = ::concrete::minimum<value_type, Compare>;

		static constexpr bool associative{true};
		static constexpr bool abelian{true};

		using algebraic_structure_tag = ::concrete::abelian_semigroup_tag;
	};

}