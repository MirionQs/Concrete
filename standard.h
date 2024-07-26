#pragma once

#include <climits>
#include <utility>

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace concrete {

	constexpr bool is_constant_evaluated() noexcept {
		return __builtin_is_constant_evaluated();
	}

	template<class T>
	constexpr void swap(T& x, T& y) noexcept {
		T tmp{::std::move(x)};
		x = ::std::move(y);
		y = ::std::move(tmp);
	}

	template<class T>
	constexpr bool has_single_bit(T x) noexcept {
		return x != 0 && (x & (x - 1)) == 0;
	}

	template<class T>
	constexpr int countl_zero(T x) noexcept {
		constexpr int bit{sizeof(T) * CHAR_BIT};
		constexpr int bitu{sizeof(unsigned) * CHAR_BIT};
		constexpr int bitul{sizeof(unsigned long) * CHAR_BIT};
		constexpr int bitull{sizeof(unsigned long long) * CHAR_BIT};

		if (::concrete::is_constant_evaluated()) {
			if (x == 0) {
				return bit;
			}
			int res{};
			while (x >> (bit - 1) == 0) {
				x <<= 1;
				++res;
			}
			return res;
		}

#ifdef _MSC_VER
		unsigned long res{};
		if constexpr (bit <= bitul) {
			return _BitScanReverse(&res, x) ? bit - 1 - res : bit;
		}
		return _BitScanReverse64(&res, x) ? bit - 1 - res : bit;
#else
		if (x == 0) {
			return bit;
		}
		if constexpr (bit <= bitu) {
			return __builtin_clz(x) - (bitu - bit);
		}
		if constexpr (bit <= bitul) {
			return __builtin_clzl(x) - (bitul - bit);
		}
		return __builtin_clzll(x) - (bitull - bit);
#endif
	}

	template<class T>
	constexpr int countr_zero(T x) noexcept {
		constexpr int bit{sizeof(T) * CHAR_BIT};
		constexpr int bitu{sizeof(unsigned) * CHAR_BIT};
		constexpr int bitul{sizeof(unsigned long) * CHAR_BIT};
		constexpr int bitull{sizeof(unsigned long long) * CHAR_BIT};

		if (::concrete::is_constant_evaluated()) {
			if (x == 0) {
				return bit;
			}
			int res{};
			while ((x & 1) == 0) {
				x >>= 1;
				++res;
			}
			return res;
		}

#ifdef _MSC_VER
		unsigned long res{};
		if constexpr (bit <= bitul) {
			return _BitScanForward(&res, x) ? res : bit;
		}
		return _BitScanForward64(&res, x) ? res : bit;
#else
		if (x == 0) {
			return bit;
		}
		if constexpr (bit <= bitu) {
			return __builtin_ctz(x);
		}
		if constexpr (bit <= bitul) {
			return __builtin_ctzl(x);
		}
		return __builtin_ctzll(x);
#endif
	}

	template<class T>
	constexpr int bit_width(T x) noexcept {
		return sizeof(T) * CHAR_BIT - ::concrete::countl_zero(x);
	}

	template<class T>
	constexpr T bit_ceil(T x) noexcept {
		if (x == 0 || x == 1) {
			return 1;
		}
		return T{1} << ::concrete::bit_width(T(x - 1));
	}

	template<class T>
	constexpr T bit_floor(T x) noexcept {
		if (x == 0) {
			return 0;
		}
		return T{1} << (::concrete::bit_width(x) - 1);
	}

}