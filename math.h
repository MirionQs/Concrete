#pragma once

#include "stdint.h"

#include <compare>

namespace concrete {

	constexpr bool is_even(::std::integral auto n) {
		return (n & 1) == 0;
	}

	constexpr bool is_odd(::std::integral auto n) {
		return (n & 1) != 0;
	}

	template<uint32_64_t auto _m>
		requires(is_odd(_m) && _m >> ((sizeof(_m) << 3) - 2) == 0)
	struct int_p {
		using value_type = decltype(_m);
		using _valueTypeDouble = double_size_t<value_type>;

		static constexpr size_t _valueTypeSize = sizeof(value_type) << 3;
		static constexpr value_type _mDouble{_m << 1};
		static constexpr value_type _mInverseNegate{[] {
			value_type res{1};
			for (size_t i{0}; i != 6; ++i) {
				res *= 2 - res * _m;
			}
			return ~res + 1;
		}()};
		static constexpr value_type _rSquare{(~_valueTypeDouble{_m} + 1) % _m};

		static constexpr value_type _reduce(_valueTypeDouble v) noexcept {
			return (v + _valueTypeDouble{static_cast<value_type>(v) * _mInverseNegate} *_m) >> _valueTypeSize;
		}

		value_type _value;

		constexpr int_p(value_type v = 0) noexcept {
			_value = _reduce(_valueTypeDouble{v} *_rSquare);
		}

		explicit constexpr operator value_type() const noexcept {
			value_type res{_reduce(_value)};
			value_type t{res - _m};
			if (t >> (_valueTypeSize - 1) == 0) {
				res = t;
			}
			return res;
		}

		constexpr ::std::strong_ordering operator<=>(int_p p) const noexcept {
			return value_type{*this} <=> value_type{p};
		}

		constexpr int_p& negate() noexcept {
			if (_value != 0) {
				_value = _mDouble - _value;
			}
			return *this;
		}

		constexpr int_p& operator+=(int_p p) noexcept {
			_value += p._value;
			value_type t{_value - _mDouble};
			if (t >> (_valueTypeSize - 1) == 0) {
				_value = t;
			}
			return *this;
		}

		constexpr int_p& operator-=(int_p p) noexcept {
			_value -= p._value;
			if (_value >> (_valueTypeSize - 1) != 0) {
				_value += _mDouble;
			}
			return *this;
		}

		constexpr int_p& operator*=(int_p p) noexcept {
			_value = _reduce(_valueTypeDouble{_value} *p._value);
			return *this;
		}

		constexpr int_p& operator/=(int_p p) noexcept {
			value_type v{_m - 2};
			while (v != 0) {
				if (v & 1) {
					*this *= p;
				}
				p *= p;
				v >>= 1;
			}
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

		constexpr int_p& operator++() noexcept {
			++_value;
			if (_value == _mDouble) {
				_value = 0;
			}
			return *this;
		}

		constexpr int_p operator++(int) noexcept {
			int_p t{*this};
			++*this;
			return t;
		}

		constexpr int_p& operator--() noexcept {
			if (_value == 0) {
				_value = _mDouble;
			}
			--_value;
			return *this;
		}

		constexpr int_p operator--(int) noexcept {
			int_p t{*this};
			--*this;
			return t;
		}
	};

	template<uint32_64_t auto _m>
	constexpr int_p<_m> operator+(decltype(_m) v, int_p<_m> p) noexcept {
		return int_p<_m>{v} += p;
	}

	template<uint32_64_t auto _m>
	constexpr int_p<_m> operator-(decltype(_m) v, int_p<_m> p) noexcept {
		return int_p<_m>{v} -= p;
	}

	template<uint32_64_t auto _m>
	constexpr int_p<_m> operator*(decltype(_m) v, int_p<_m> p) noexcept {
		return int_p<_m>{v} *= p;
	}

	template<uint32_64_t auto _m>
	constexpr int_p<_m> operator/(decltype(_m) v, int_p<_m> p) noexcept {
		return int_p<_m>{v} /= p;
	}

	template<uint32_64_t auto _m>
	constexpr int_p<_m> power(int_p<_m> p, ::std::uint64_t v) noexcept {
		int_p<_m> res{1};
		while (v != 0) {
			if (v & 1) {
				res *= p;
			}
			p *= p;
			v >>= 1;
		}
		return res;
	}

	template<uint32_64_t auto _m>
	constexpr int_p<_m> inverse(int_p<_m> p) noexcept {
		return power(p, _m - 2);
	}

}