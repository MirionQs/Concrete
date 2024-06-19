#pragma once

#include "integer.h"

#include <bit>
#include <compare>

namespace concrete {

	template<::concrete::integral T>
	class montgomery {
	public:
		using value_type = ::concrete::make_unsigned_t<T>;

	private:
		using _doubleType = ::concrete::unsigned_integer_t<sizeof(value_type) * 2>;

		const value_type _m, _mDouble, _mInverseNegate, _r, _rSquare;

		constexpr value_type _inverse(value_type m) const noexcept {
			value_type res{1};
			for (std::size_t i{0}; i != ::std::countr_zero(sizeof(value_type) * 8); ++i) {
				res *= 2 - res * m;
			}
			return res;
		}

		constexpr value_type _reduce(_doubleType x) const noexcept {
			return value_type{(x + _doubleType{(value_type)x * _mInverseNegate} *_m) >> (sizeof(value_type) * 8)};
		}

	public:
		explicit constexpr montgomery(value_type m) noexcept :
			_m{m},
			_mDouble{m << 1},
			_mInverseNegate{~_inverse(m) + 1},
			_r{(~value_type{m} + 1) % m},
			_rSquare{(~_doubleType{m} + 1) % m} {}

		constexpr value_type modulo() const noexcept {
			return _m;
		}

		constexpr value_type operator()(value_type x) const noexcept {
			return _reduce(_doubleType{x} *_rSquare);
		}

		constexpr value_type to(value_type xR) const noexcept {
			value_type res{_reduce(xR)};
			value_type t{res - _m};
			return t >> (sizeof(value_type) * 8 - 1) == 0 ? t : res;
		}

		constexpr ::std::strong_ordering compare(value_type xR, value_type yR) const noexcept {
			return to(xR) <=> to(yR);
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
			return _reduce(_doubleType{xR} *yR);
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
	explicit montgomery(T)->montgomery<T>;

	template<::concrete::integral auto m>
		requires(m % 2 != 0 && m >> (sizeof(m) * 8 - 2) == 0)
	class int_mod {
	public:
		using value_type = ::concrete::make_unsigned_t<decltype(m)>;
		using montgomery_type = ::concrete::montgomery<value_type>;

	private:
		static constexpr montgomery_type _mont{m};

		value_type _value;

	public:
		constexpr int_mod(value_type v = 0) noexcept {
			_value = _mont(v);
		}

		explicit constexpr operator value_type() const noexcept {
			return _mont.to(_value);
		}

		constexpr const montgomery_type& montgomery() const noexcept {
			return _mont;
		}

		constexpr value_type raw() const noexcept {
			return _value;
		}

		constexpr void raw(value_type v) noexcept {
			_value = v;
		}

		constexpr ::std::strong_ordering operator<=>(int_mod x) const noexcept {
			return _mont.compare(_value, x._value);
		}

		constexpr int_mod& negate() noexcept {
			_value = _mont.negate(_value);
			return *this;
		}

		constexpr int_mod& operator+=(int_mod x) noexcept {
			_value = _mont.add(_value, x._value);
			return *this;
		}

		constexpr int_mod& operator-=(int_mod x) noexcept {
			_value = _mont.subtract(_value, x._value);
			return *this;
		}

		constexpr int_mod& operator*=(int_mod x) noexcept {
			_value = _mont.multiply(_value, x._value);
			return *this;
		}

		constexpr int_mod& operator/=(int_mod x) noexcept {
			_value = _mont.divide(_value, x._value);
			return *this;
		}

		constexpr int_mod operator+() const noexcept {
			return *this;
		}

		constexpr int_mod operator-() const noexcept {
			return int_mod{*this}.negate();
		}

		constexpr int_mod operator+(int_mod x) const noexcept {
			return int_mod{*this} += x;
		}

		constexpr int_mod operator-(int_mod x) const noexcept {
			return int_mod{*this} -= x;
		}

		constexpr int_mod operator*(int_mod x) const noexcept {
			return int_mod{*this} *= x;
		}

		constexpr int_mod operator/(int_mod x) const noexcept {
			return int_mod{*this} /= x;
		}
	};

	template<auto m>
	constexpr int_mod<m> operator+(typename int_mod<m>::value_type v, int_mod<m> x) noexcept {
		return int_mod<m>{v} += x;
	}

	template<auto m>
	constexpr int_mod<m> operator-(typename int_mod<m>::value_type v, int_mod<m> x) noexcept {
		return int_mod<m>{v} -= x;
	}

	template<auto m>
	constexpr int_mod<m> operator*(typename int_mod<m>::value_type v, int_mod<m> x) noexcept {
		return int_mod<m>{v} *= x;
	}

	template<auto m>
	constexpr int_mod<m> operator/(typename int_mod<m>::value_type v, int_mod<m> x) noexcept {
		return int_mod<m>{v} /= x;
	}

	template<auto m>
	constexpr int_mod<m> power(int_mod<m> x, typename int_mod<m>::value_type v) noexcept {
		x.raw(x.montgomery().power(x.raw(), v));
		return x;
	}

	template<auto m>
	constexpr int_mod<m> inverse(int_mod<m> x) noexcept {
		x.raw(x.montgomery().inverse(x.raw()));
		return x;
	}

}