#pragma once

#include "stdint.h"

#include <concepts>
#include <compare>
#include <bit>

namespace concrete {

	template<::std::unsigned_integral T>
	class montgomery {
	public:
		using value_type = T;

	private:
		using _doubleValueType = double_size_t<value_type>;

		const value_type _m, _mDouble, _mInverseNegate, _r, _rSquare;

		constexpr value_type _inverse(value_type m) const noexcept {
			constexpr size_t s{::std::countr_zero(sizeof(value_type) * 8)};
			value_type res{1};
			for (size_t i{0}; i != s; ++i) {
				res *= 2 - res * m;
			}
			return res;
		}

		constexpr value_type _reduce(_doubleValueType x) const noexcept {
			constexpr size_t s{sizeof(value_type) * 8};
			return value_type{(x + _doubleValueType{static_cast<value_type>(x) * _mInverseNegate} *_m) >> s};
		}

		constexpr value_type _power(value_type xR, value_type y, value_type zR) const noexcept {
			while (y != 0) {
				if (y & 1) {
					zR = multiply(zR, xR);
				}
				xR = multiply(xR, xR);
				y >>= 1;
			}
			return zR;
		}

	public:
		explicit constexpr montgomery(value_type m) noexcept :
			_m{m},
			_mDouble{m << 1},
			_mInverseNegate{~_inverse(m) + 1},
			_r{(~value_type{m} + 1) % m},
			_rSquare{(~_doubleValueType{m} + 1) % m} {}

		constexpr value_type modulo() const noexcept {
			return _m;
		}

		constexpr value_type from(value_type x) const noexcept {
			return _reduce(_doubleValueType{x} *_rSquare);
		}

		constexpr value_type to(value_type xR) const noexcept {
			constexpr size_t s{sizeof(value_type) * 8 - 1};
			value_type res{_reduce(xR)};
			value_type t{res - _m};
			return t >> s == 0 ? t : res;
		}

		constexpr ::std::strong_ordering compare(value_type xR, value_type yR) const noexcept {
			return to(xR) <=> to(yR);
		}

		constexpr value_type negate(value_type xR) const noexcept {
			return xR == 0 ? 0 : _mDouble - xR;
		}

		constexpr value_type add(value_type xR, value_type yR) const noexcept {
			constexpr size_t s{sizeof(value_type) * 8 - 1};
			xR += yR;
			value_type t{xR - _mDouble};
			return t >> s == 0 ? t : xR;
		}

		constexpr value_type subtract(value_type xR, value_type yR) const noexcept {
			constexpr size_t s{sizeof(value_type) * 8 - 1};
			xR -= yR;
			return xR >> s == 0 ? xR : xR + _mDouble;
		}

		constexpr value_type multiply(value_type xR, value_type yR) const noexcept {
			return _reduce(_doubleValueType{xR} *yR);
		}

		constexpr value_type divide(value_type xR, value_type yR) const noexcept {
			return _power(yR, _m - 2, xR);
		}

		constexpr value_type power(value_type xR, value_type y) const noexcept {
			return _power(xR, y, _r);
		}

		constexpr value_type inverse(value_type xR) const noexcept {
			return power(xR, _m - 2);
		}
	};

	template<class T>
	explicit montgomery(T)->montgomery<::std::make_unsigned_t<T>>;

	template<::std::unsigned_integral auto m>
		requires(m % 2 != 0 && m >> (sizeof(m) * 8 - 2) == 0)
	class int_p {
	public:
		using value_type = decltype(m);
		using montgomery_type = ::concrete::montgomery<value_type>;

	private:
		static constexpr montgomery_type _mont{m};

		value_type _value;

	public:
		constexpr int_p(value_type v = 0) noexcept {
			_value = _mont.from(v);
		}

		explicit constexpr operator value_type() const noexcept {
			return _mont.to(_value);
		}

		constexpr const montgomery_type& montgomery() const noexcept {
			return _mont;
		}

		constexpr value_type& raw() noexcept {
			return _value;
		}

		constexpr ::std::strong_ordering operator<=>(int_p p) const noexcept {
			return _mont.compare(_value, p._value);
		}

		constexpr int_p& negate() noexcept {
			_value = _mont.negate(_value);
			return *this;
		}

		constexpr int_p& operator+=(int_p p) noexcept {
			_value = _mont.add(_value, p._value);
			return *this;
		}

		constexpr int_p& operator-=(int_p p) noexcept {
			_value = _mont.subtract(_value, p._value);
			return *this;
		}

		constexpr int_p& operator*=(int_p p) noexcept {
			_value = _mont.multiply(_value, p._value);
			return *this;
		}

		constexpr int_p& operator/=(int_p p) noexcept {
			_value = _mont.divide(_value, p._value);
			return *this;
		}

		constexpr int_p operator+() const noexcept {
			return *this;
		}

		constexpr int_p operator-() const noexcept {
			return int_p{*this}.negate();
		}

		constexpr int_p operator+(int_p p) const noexcept {
			return int_p{*this} += p;
		}

		constexpr int_p operator-(int_p p) const noexcept {
			return int_p{*this} -= p;
		}

		constexpr int_p operator*(int_p p) const noexcept {
			return int_p{*this} *= p;
		}

		constexpr int_p operator/(int_p p) const noexcept {
			return int_p{*this} /= p;
		}
	};

	template<auto m>
	constexpr int_p<m> operator+(typename int_p<m>::value_type v, int_p<m> p) noexcept {
		return int_p<m>{v} += p;
	}

	template<auto m>
	constexpr int_p<m> operator-(typename int_p<m>::value_type v, int_p<m> p) noexcept {
		return int_p<m>{v} -= p;
	}

	template<auto m>
	constexpr int_p<m> operator*(typename int_p<m>::value_type v, int_p<m> p) noexcept {
		return int_p<m>{v} *= p;
	}

	template<auto m>
	constexpr int_p<m> operator/(typename int_p<m>::value_type v, int_p<m> p) noexcept {
		return int_p<m>{v} /= p;
	}

	template<auto m>
	constexpr int_p<m> power(int_p<m> p, typename int_p<m>::value_type v) noexcept {
		p.raw() = p.montgomery().power(p.raw(), v);
		return p;
	}

	template<auto m>
	constexpr int_p<m> inverse(int_p<m> p) noexcept {
		p.raw() = p.montgomery().inverse(p.raw());
		return p;
	}

}