#pragma once

#include "int_p.h"

#include <initializer_list>
#include <cmath>

namespace concrete {

	namespace detail {

		namespace is_prime {

			using data_type = ::std::initializer_list<uint64_t>;

			constexpr data_type _mrBases1{9345883071009581737};
			constexpr data_type _mrBases2{336781006125, 9639812373923155};
			constexpr data_type _mrBases3{4230279247111683200, 14694767155120705706, 16641139526367750375};
			constexpr data_type _mrBases4{2, 141889084524735, 1199124725622454117, 11096072698276303650};
			constexpr data_type _mrBases5{2, 4130806001517, 149795463772692060, 186635894390467037, 3967304179347715805};
			constexpr data_type _mrBases6{2, 123635709730000, 9233062284813009, 43835965440333360, 761179012939631437, 1263739024124850375};
			constexpr data_type _mrBases7{2, 325, 9375, 28178, 450775, 9780504, 1795265022};

			constexpr data_type _mrPrimes1{47, 98207, 2024790248953};
			constexpr data_type _mrPrimes2{131, 6855593, 1927962474784631};
			constexpr data_type _mrPrimes3{19, 29, 277, 991, 1931, 14347, 14683, 246557, 3709689913};
			constexpr data_type _mrPrimes4{11, 23, 127, 56197, 3075593, 322232233, 3721305949};
			constexpr data_type _mrPrimes5{13, 29, 59, 79, 167, 62633, 299197, 2422837, 332721269, 560937673};
			constexpr data_type _mrPrimes6{13, 41, 61, 179, 1381, 30839, 157321, 385417, 627838711, 1212379867, 7985344259};
			constexpr data_type _mrPrimes7{13, 19, 73, 193, 407521, 299210837};

			constexpr data_type miller_rabin_bases[]{_mrBases1, _mrBases2, _mrBases3, _mrBases4, _mrBases5, _mrBases6, _mrBases7};
			constexpr data_type miller_rabin_primes[]{_mrPrimes1, _mrPrimes2, _mrPrimes3, _mrPrimes4, _mrPrimes5, _mrPrimes6, _mrPrimes7};
			constexpr data_type miller_rabin_bounds{341531, 1050535501, 350269456337, 55245642489451, 7999252175582851, 585226005592931977};

			constexpr bool miller_rabin(uint64_t x, data_type bases) {
				montgomery<uint64_t> mont{x};
				unsigned n{static_cast<unsigned>(::std::countr_zero(x - 1))};
				uint64_t c{(x - 1) >> n};
				for (uint64_t b : bases) {
					uint64_t t{mont.power(mont.from(b), c)};
					if (mont.to(t) != 1) {
						unsigned k{0};
						while (mont.to(t) != x - 1) {
							t = mont.multiply(t, t);
							if (++k == n) {
								return false;
							}
						}
					}
				}
				return true;
			}

		}

	}

	constexpr uint64_t square_root(uint64_t x) noexcept {
		if (::std::is_constant_evaluated()) {
			if (x == 0) {
				return x;
			}
			unsigned n{32 - (static_cast<unsigned>(::std::countl_zero(x - 1)) >> 1)};
			uint64_t x0{uint64_t{1} << n}, x1{(x0 + (x >> n)) >> 1};
			while (x0 > x1) {
				x0 = x1;
				x1 = (x0 + x / x0) >> 1;
			}
			return x0;
		}
		return static_cast<uint64_t>(::std::floor(::std::sqrt(x)) + .5);
	}

	constexpr uint64_t power(uint64_t x, uint64_t y) noexcept {
		if (x == 0) {
			return y == 0;
		}
		if (x == 1) {
			return 1;
		}
		uint64_t res{1};
		while (y != 0) {
			uint64_t x2{x * x}, x3{x2 * x};
			if ((y & 3) == 1) {
				res *= x;
			}
			else if ((y & 3) == 2) {
				res *= x2;
			}
			else if ((y & 3) == 3) {
				res *= x3;
			}
			x *= x3;
			y >>= 2;
		}
		return res;
	}

	constexpr uint64_t greatest_common_divisor(uint64_t x, uint64_t y) noexcept {
		if (x == 0) {
			return y;
		}
		if (y == 0) {
			return x;
		}
		unsigned xn{static_cast<unsigned>(::std::countr_zero(x))};
		unsigned yn{static_cast<unsigned>(::std::countr_zero(y))};
		x >>= xn;
		y >>= yn;
		while (true) {
			if (x < y) {
				::std::swap(x, y);
			}
			x -= y;
			if (x == 0) {
				return y << ::std::min(xn, yn);
			}
			x >>= ::std::countr_zero(x);
		}
	}

	constexpr uint64_t least_common_multiple(uint64_t x, uint64_t y) noexcept {
		return x / greatest_common_divisor(x, y) * y;
	}

	constexpr int kronecker_symbol(uint64_t x, uint64_t y) noexcept {
		if (x == 0) {
			return y == 1;
		}
		if (y == 0) {
			return x == 1;
		}
		if (((x | y) & 1) == 0) {
			return 0;
		}
		unsigned xn, yn;
		bool m{true};
		if ((x & 1) == 0) {
			xn = static_cast<unsigned>(::std::countr_zero(x));
			m = (xn & 1) == 0 || (y & 7) == 1 || (y & 7) == 7;
			x >>= xn;
		}
		else if ((y & 1) == 0) {
			yn = static_cast<unsigned>(::std::countr_zero(y));
			m = (yn & 1) == 0 || (x & 7) == 1 || (x & 7) == 7;
			y >>= yn;
		}
		while (true) {
			if (x < y) {
				::std::swap(x, y);
				m = m == ((x & y & 2) == 0);
			}
			x -= y;
			if (x == 0) {
				return y != 1 ? 0 : m ? 1 : -1;
			}
			xn = static_cast<unsigned>(::std::countr_zero(x));
			m = m == ((xn & 1) == 0 || (y & 7) == 1 || (y & 7) == 7);
			x >>= xn;
		}
	}

	constexpr bool is_prime(uint64_t x) noexcept {
		using namespace detail::is_prime;
		if (x == 0 || x == 1) {
			return false;
		}
		if (x == 2 || x == 3 || x == 5 || x == 7) {
			return true;
		}
		if ((x & 1) == 0 || x % 3 == 0 || x % 5 == 0 || x % 7 == 0) {
			return false;
		}
		size_t i{0};
		for (uint64_t b : miller_rabin_bounds) {
			if (x < b) {
				break;
			}
			++i;
		}
		for (uint64_t p : miller_rabin_primes[i]) {
			if (x == p) {
				return true;
			}
			if (x % p == 0) {
				return false;
			}
		}
		return miller_rabin(x, miller_rabin_bases[i]);
	}

}