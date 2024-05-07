#pragma once

#include <compare>
#include <cstdint>
#include <cstddef>

namespace concrete {

	template<::std::uint32_t _p>
		requires(_p < (1 << 30) && (_p & 1) == 1)
	struct int_p {
		::std::uint32_t _value;

		static constexpr ::std::uint32_t _pDouble{_p << 1};
		static constexpr ::std::uint32_t _pInverseNegate{[] {
			::std::uint32_t res{1};
			for (::std::size_t i{0}; i != 6; ++i) {
				res *= 2 - res * _p;
			}
			return ~res + 1;
		}()};
		static constexpr ::std::uint32_t _rSquare{(~::std::uint64_t{_p} + 1) % _p};

		static constexpr ::std::uint32_t _reduce(::std::uint64_t v) noexcept {
			return (v + ::std::uint64_t{static_cast<::std::uint32_t>(v) * _pInverseNegate} *_p) >> 32;
		}

		constexpr int_p(::std::uint32_t v = 0) noexcept {
			_value = _reduce(::std::uint64_t{v} *_rSquare);
		}

		explicit constexpr operator ::std::uint32_t() const noexcept {
			::std::uint32_t res{_reduce(_value)};
			::std::uint32_t t{res - _p};
			if (t >> 31 == 0) {
				res = t;
			}
			return res;
		}

		constexpr ::std::strong_ordering operator<=>(int_p p) const noexcept {
			return ::std::uint32_t{*this} <=> ::std::uint32_t{p};
		}

		constexpr int_p& negate() noexcept {
			if (_value != 0) {
				_value = _pDouble - _value;
			}
			return *this;
		}

		constexpr int_p& operator+=(int_p p) noexcept {
			_value += p._value;
			::std::uint32_t t{_value - _pDouble};
			if (t >> 31 == 0) {
				_value = t;
			}
			return *this;
		}

		constexpr int_p& operator-=(int_p p) noexcept {
			_value -= p._value;
			if (_value >> 31 != 0) {
				_value += _pDouble;
			}
			return *this;
		}

		constexpr int_p& operator*=(int_p p) noexcept {
			_value = _reduce(::std::uint64_t{_value} *p._value);
			return *this;
		}

		constexpr int_p& operator/=(int_p p) noexcept {
			::std::uint32_t v{_p - 2};
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
			if (_value == _pDouble) {
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
				_value = _pDouble;
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

	template<::std::uint32_t _p>
	constexpr int_p<_p> operator+(::std::uint32_t v, int_p<_p> p) noexcept {
		return int_p<_p>{v} += p;;
	}

	template<::std::uint32_t _p>
	constexpr int_p<_p> operator-(::std::uint32_t v, int_p<_p> p) noexcept {
		return int_p<_p>{v} -= p;
	}

	template<::std::uint32_t _p>
	constexpr int_p<_p> operator*(::std::uint32_t v, int_p<_p> p) noexcept {
		return int_p<_p>{v} *= p;
	}

	template<::std::uint32_t _p>
	constexpr int_p<_p> operator/(::std::uint32_t v, int_p<_p> p) noexcept {
		return int_p<_p>{v} /= p;
	}

	template<::std::uint32_t _p>
	constexpr int_p<_p> power(int_p<_p> p, ::std::uint32_t v) noexcept {
		int_p<_p> res{1};
		while (v != 0) {
			if (v & 1) {
				res *= p;
			}
			p *= p;
			v >>= 1;
		}
		return res;
	}

	template<::std::uint32_t _p>
	constexpr int_p<_p> inverse(int_p<_p> p) noexcept {
		return ::concrete::power(p, _p - 2);
	}

}