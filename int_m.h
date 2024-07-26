#pragma once

#include "integral.h"
#include "standard.h"

namespace concrete {

	template<class T>
	class modular_arithmetic {
		static_assert(::concrete::is_integral_v<T>, "T should be an integral type.");

	public:
		using value_type = ::concrete::make_unsigned_t<T>;

	private:
		using _double = ::concrete::unsigned_integral_t<sizeof(value_type) * 2>;

		const value_type _m, _mDouble, _mInverseNegate, _r, _rSquare;

		constexpr value_type _inverse(value_type x) const noexcept {
			value_type res{1};
			constexpr ::std::size_t n{::concrete::countr_zero(sizeof(value_type) * 8)};
			for (::std::size_t i{0}; i != n; ++i) {
				res *= 2 - res * x;
			}
			return res;
		}

		constexpr value_type _reduce(_double x) const noexcept {
			return value_type{(x + _double{(value_type)x * _mInverseNegate} *_m) >> (sizeof(value_type) * 8)};
		}

	public:
		explicit constexpr modular_arithmetic(value_type modulo) noexcept :
			_m{modulo},
			_mDouble{modulo << 1},
			_mInverseNegate{~_inverse(modulo) + 1},
			_r{(~value_type{modulo} + 1) % modulo},
			_rSquare{(~_double{modulo} + 1) % modulo} {}

		constexpr value_type modulo() const noexcept {
			return _m;
		}

		constexpr value_type operator()(value_type x) const noexcept {
			return _reduce(_double{x} *_rSquare);
		}

		constexpr value_type to(value_type xR) const noexcept {
			value_type res{_reduce(xR)};
			value_type t{res - _m};
			return t >> (sizeof(value_type) * 8 - 1) == 0 ? t : res;
		}

		constexpr bool equal(value_type xR, value_type yR) const noexcept {
			return to(xR) == to(yR);
		}

		constexpr bool not_equal(value_type xR, value_type yR) const noexcept {
			return to(xR) != to(yR);
		}

		constexpr bool less(value_type xR, value_type yR) const noexcept {
			return to(xR) < to(yR);
		}

		constexpr bool less_equal(value_type xR, value_type yR) const noexcept {
			return to(xR) <= to(yR);
		}

		constexpr bool greater(value_type xR, value_type yR) const noexcept {
			return to(xR) > to(yR);
		}

		constexpr bool greater_equal(value_type xR, value_type yR) const noexcept {
			return to(xR) >= to(yR);
		}

		constexpr value_type negate(value_type xR) const noexcept {
			return xR == 0 ? 0 : _mDouble - xR;
		}

		constexpr value_type add(value_type xR, value_type yR) const noexcept {
			xR += yR;
			value_type t{xR - _mDouble};
			return t >> (sizeof(value_type) * 8 - 1) == 0 ? t : xR;
		}

		constexpr value_type subtract(value_type xR, value_type yR) const noexcept {
			xR -= yR;
			return xR >> (sizeof(value_type) * 8 - 1) == 0 ? xR : xR + _mDouble;
		}

		constexpr value_type multiply(value_type xR, value_type yR) const noexcept {
			return _reduce(_double{xR} *yR);
		}

		constexpr value_type divide(value_type xR, value_type yR) const noexcept {
			return power_multiply(yR, _m - 2, xR);
		}

		constexpr value_type power_multiply(value_type xR, value_type y, value_type zR) const noexcept {
			while (y != 0) {
				if (y & 1) {
					zR = multiply(zR, xR);
				}
				xR = multiply(xR, xR);
				y >>= 1;
			}
			return zR;
		}

		constexpr value_type power(value_type xR, value_type y) const noexcept {
			return power_multiply(xR, y, _r);
		}

		constexpr value_type inverse(value_type xR) const noexcept {
			return power_multiply(xR, _m - 2, _r);
		}
	};

	template<class T>
	explicit modular_arithmetic(T)->modular_arithmetic<T>;

	template<auto m>
	class int_m {
		static_assert(::concrete::is_integral_v<decltype(m)>&& m % 2 != 0 && m >> (sizeof(m) * 8 - 2) == 0,
			"m should be an odd number less than 2 ^ (sizeof(m) * 8 - 2).");

	public:
		using value_type = ::concrete::make_unsigned_t<decltype(m)>;
		using modular_arithmetic_type = ::concrete::modular_arithmetic<value_type>;

	private:
		static constexpr modular_arithmetic_type _mod{m};

		value_type _value;

	public:
		constexpr int_m(value_type value = 0) noexcept {
			_value = _mod(value);
		}

		explicit constexpr operator value_type() const noexcept {
			return _mod.to(_value);
		}

		constexpr const modular_arithmetic_type& modular_arithmetic() const noexcept {
			return _mod;
		}

		constexpr value_type raw() const noexcept {
			return _value;
		}

		constexpr void raw(value_type value) noexcept {
			_value = value;
		}

		constexpr bool operator==(int_m x) const noexcept {
			return _mod.equal(_value, x._value);
		}

		constexpr bool operator!=(int_m x) const noexcept {
			return _mod.not_equal(_value, x._value);
		}

		constexpr bool operator<(int_m x) const noexcept {
			return _mod.less(_value, x._value);
		}

		constexpr bool operator<=(int_m x) const noexcept {
			return _mod.less_equal(_value, x._value);
		}

		constexpr bool operator>(int_m x) const noexcept {
			return _mod.greater(_value, x._value);
		}

		constexpr bool operator>=(int_m x) const noexcept {
			return _mod.greater_equal(_value, x._value);
		}

		constexpr int_m& negate() noexcept {
			_value = _mod.negate(_value);
			return *this;
		}

		constexpr int_m& operator+=(int_m x) noexcept {
			_value = _mod.add(_value, x._value);
			return *this;
		}

		constexpr int_m& operator-=(int_m x) noexcept {
			_value = _mod.subtract(_value, x._value);
			return *this;
		}

		constexpr int_m& operator*=(int_m x) noexcept {
			_value = _mod.multiply(_value, x._value);
			return *this;
		}

		constexpr int_m& operator/=(int_m x) noexcept {
			_value = _mod.divide(_value, x._value);
			return *this;
		}

		constexpr int_m operator+() const noexcept {
			return *this;
		}

		constexpr int_m operator-() const noexcept {
			return int_m{*this}.negate();
		}

		constexpr int_m operator+(int_m x) const noexcept {
			return int_m{*this} += x;
		}

		constexpr int_m operator-(int_m x) const noexcept {
			return int_m{*this} -= x;
		}

		constexpr int_m operator*(int_m x) const noexcept {
			return int_m{*this} *= x;
		}

		constexpr int_m operator/(int_m x) const noexcept {
			return int_m{*this} /= x;
		}
	};

	template<auto m>
	constexpr int_m<m> operator+(typename int_m<m>::value_type value, int_m<m> x) noexcept {
		return int_m<m>{value} += x;
	}

	template<auto m>
	constexpr int_m<m> operator-(typename int_m<m>::value_type value, int_m<m> x) noexcept {
		return int_m<m>{value} -= x;
	}

	template<auto m>
	constexpr int_m<m> operator*(typename int_m<m>::value_type value, int_m<m> x) noexcept {
		return int_m<m>{value} *= x;
	}

	template<auto m>
	constexpr int_m<m> operator/(typename int_m<m>::value_type value, int_m<m> x) noexcept {
		return int_m<m>{value} /= x;
	}

	template<auto m>
	constexpr int_m<m> power(int_m<m> x, typename int_m<m>::value_type value) noexcept {
		x.raw(x.modular_arithmetic().power(x.raw(), value));
		return x;
	}

	template<auto m>
	constexpr int_m<m> inverse(int_m<m> x) noexcept {
		x.raw(x.modular_arithmetic().inverse(x.raw()));
		return x;
	}

}