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

	template<class Op>
	struct inverse_operator {};

	template<class T>
	struct inverse_operator<::concrete::add<T>> {
		using type = ::concrete::subtract<T>;
	};

	template<class T>
	struct inverse_operator<::concrete::multiply<T>> {
		using type = ::concrete::divide<T>;
	};

	template<class Op>
	using inverse_operator_t = typename ::concrete::inverse_operator<Op>::type;

	template<class Op>
	struct inverse_assignment_operator {
		using type = ::concrete::assignment_operator_t<::concrete::inverse_operator_t<Op>>;
	};

	template<class Op>
	using inverse_assignment_operator_t = typename ::concrete::inverse_assignment_operator<Op>::type;

	struct set_tag {};
	struct magma_tag : ::concrete::set_tag {};
	struct semigroup_tag : ::concrete::magma_tag {};
	struct monoid_tag : ::concrete::semigroup_tag {};
	struct group_tag : ::concrete::monoid_tag {};

	template<class Op>
	struct algebraic_traits {
		static constexpr bool associative{false};
		static constexpr bool invertible{false};
		static constexpr bool commutative{false};
		using algebraic_structure_tag = ::concrete::set_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::add<T>> {
		static constexpr bool associative{true};
		static constexpr T identity{0};
		static constexpr bool invertible{true};
		static constexpr bool commutative{true};
		using algebraic_structure_tag = ::concrete::group_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::subtract<T>> {
		static constexpr bool associative{false};
		static constexpr bool invertible{false};
		static constexpr bool commutative{false};
		using algebraic_structure_tag = ::concrete::magma_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::multiply<T>> {
		static constexpr bool associative{true};
		static constexpr T identity{1};
		static constexpr bool invertible{true};
		static constexpr bool commutative{true};
		using algebraic_structure_tag = ::concrete::group_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::divide<T>> {
		static constexpr bool associative{false};
		static constexpr bool invertible{false};
		static constexpr bool commutative{false};
		using algebraic_structure_tag = ::concrete::magma_tag;
	};

	template<class T>
	struct algebraic_traits<::concrete::modulo<T>> {
		static constexpr bool associative{false};
		static constexpr bool invertible{false};
		static constexpr bool commutative{false};
		using algebraic_structure_tag = ::concrete::magma_tag;
	};

	template<class Op>
	constexpr bool is_magma_v{::std::is_base_of_v<::concrete::magma_tag, typename ::concrete::algebraic_traits<Op>::algebraic_structure_tag>};

	template<class Op>
	constexpr bool is_semigroup_v{::std::is_base_of_v<::concrete::semigroup_tag, typename ::concrete::algebraic_traits<Op>::algebraic_structure_tag>};

	template<class Op>
	constexpr bool is_monoid_v{::std::is_base_of_v<::concrete::monoid_tag, typename ::concrete::algebraic_traits<Op>::algebraic_structure_tag>};

	template<class Op>
	constexpr bool is_group_v{::std::is_base_of_v<::concrete::group_tag, typename ::concrete::algebraic_traits<Op>::algebraic_structure_tag>};

	template<class Op>
	struct is_magma {
		static constexpr bool value{::concrete::is_magma_v<Op>};
	};

	template<class Op>
	struct is_semigroup {
		static constexpr bool value{::concrete::is_semigroup_v<Op>};
	};

	template<class Op>
	struct is_monoid {
		static constexpr bool value{::concrete::is_monoid_v<Op>};
	};

	template<class Op>
	struct is_group {
		static constexpr bool value{::concrete::is_group_v<Op>};
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
		static constexpr bool associative{true};
		static constexpr bool invertible{false};
		static constexpr bool commutative{true};
		using algebraic_structure_tag = ::concrete::semigroup_tag;
	};

	template<class T, class Compare>
	struct algebraic_traits<::concrete::minimum<T, Compare>> {
		static constexpr bool associative{true};
		static constexpr bool invertible{false};
		static constexpr bool commutative{true};
		using algebraic_structure_tag = ::concrete::semigroup_tag;
	};

}