#pragma once

#include "int_p.h"

#include <vector>
#include <bit>

namespace concrete {

	template<::std::uint32_t _p = 998244353, ::std::uint32_t _g = 3>
	struct polynomial : public ::std::vector<::concrete::int_p<_p>> {
		using _parent = ::std::vector<::concrete::int_p<_p>>;
		using _value = _parent::value_type;
		using _iter = _parent::iterator;
		using _citer = _parent::const_iterator;

		static inline ::std::vector<_value> _roots{1, 1};

		static void _precomputeRoots(::std::size_t n) noexcept {
			::std::size_t oldSize{_roots.size()};
			if (n <= oldSize) {
				return;
			}
			_roots.resize(n);
			static constexpr _value gInverse{::concrete::inverse(_value{_g})};
			::std::uint32_t m{_p >> ::std::countr_zero(oldSize) >> 1};
			for (::std::size_t half{oldSize}; half != n; half <<= 1, m >>= 1) {
				_value r{::concrete::power(gInverse, m)};
				for (::std::size_t i{half}; i != half << 1; i += 2) {
					_roots[i] = _roots[i >> 1];
					_roots[i + 1] = r * _roots[i];
				}
			}
		}

		static void fast_fourier_transform(_iter x, ::std::size_t n) noexcept {
			if (n == 1) {
				return;
			}
			_precomputeRoots(n);
			for (::std::size_t half{n >> 1}; half != 1; half >>= 1) {
				for (::std::size_t i{0}; i != n; i += half << 1) {
					auto r{_roots.begin() + half};
					for (::std::size_t j{i}; j != i + half; ++j) {
						_value p{x[j]}, q{x[j + half]};
						x[j] = p + q;
						x[j + half] = (p - q) * *r++;
					}
				}
			}
			for (::std::size_t i{0}; i != n; i += 2) {
				_value p{x[i]}, q{x[i + 1]};
				x[i] = p + q;
				x[i + 1] = p - q;
			}
		}

		static void inverse_fast_fourier_transform(_iter x, ::std::size_t n) noexcept {
			if (n == 1) {
				return;
			}
			_precomputeRoots(n);
			_value nInverse{_p - (_p >> ::std::countr_zero(n))};
			for (::std::size_t i{0}; i != n; i += 2) {
				_value p{x[i]}, q{x[i + 1]};
				x[i] = (p + q) * nInverse;
				x[i + 1] = (p - q) * nInverse;
			}
			for (::std::size_t half{2}; half != n; half <<= 1) {
				for (::std::size_t i{0}; i != n; i += half << 1) {
					_value p{x[i]}, q{x[i + half]};
					x[i] = p + q;
					x[i + half] = p - q;
					auto r{_roots.begin() + (half << 1)};
					for (::std::size_t j{i + 1}; j != i + half; ++j) {
						_value p{x[j]}, q{x[j + half] * *--r};
						x[j] = p - q;
						x[j + half] = p + q;
					}
				}
			}
		}

		static void negate(_iter x, ::std::size_t n) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i].negate();
			}
		}

		static void add(_iter x, ::std::size_t n, _value v) noexcept {
			x[0] += v;
		}

		static void subtract(_iter x, ::std::size_t n, _value v) noexcept {
			x[0] -= v;
		}

		static void multiply(_iter x, ::std::size_t n, _value v) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= v;
			}
		}

		static void divide(_iter x, ::std::size_t n, _value v) noexcept {
			_value vInverse{::concrete::inverse(v)};
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= vInverse;
			}
		}

		static void add(_iter x, ::std::size_t n, _citer y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] += y[i];
			}
		}

		static void subtract(_iter x, ::std::size_t n, _citer y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] -= y[i];
			}
		}

		static void multiply(_iter x, ::std::size_t n, _iter y) noexcept {
			_precomputeRoots(n);
			fast_fourier_transform(x, n);
			fast_fourier_transform(y, n);
			hadamard_product(x, n, y);
			inverse_fast_fourier_transform(x, n);
		}

		static void hadamard_product(_iter x, ::std::size_t n, _citer y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= y[i];
			}
		}

		static void inverse(_iter x, ::std::size_t n) noexcept {

		}

		using _parent::_parent;

		polynomial& negate() noexcept {
			negate(this->begin(), this->size());
			return *this;
		}

		polynomial& operator+=(::std::uint32_t v) noexcept {
			add(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator-=(::std::uint32_t v) noexcept {
			subtract(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator*=(::std::uint32_t v) noexcept {
			multiply(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator/=(::std::uint32_t v) noexcept {
			divide(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator+=(const polynomial& p) noexcept {
			add(this->begin(), this->size(), p.begin());
			return *this;
		}

		polynomial& operator-=(const polynomial& p) noexcept {
			subtract(this->begin(), this->size(), p.begin());
			return *this;
		}

		polynomial& operator*=(polynomial&& p) noexcept {
			multiply(this->begin(), this->size(), p.begin());
			return *this;
		}

		polynomial& operator*=(const polynomial& p) noexcept {
			return *this *= polynomial{p};
		}

		polynomial operator+() const noexcept {
			return *this;
		}

		polynomial operator-() const noexcept {
			return polynomial{*this}.negate();
		}

		polynomial operator+(::std::uint32_t v) const noexcept {
			return polynomial{*this} += v;
		}

		polynomial operator-(::std::uint32_t v) const noexcept {
			return polynomial{*this} -= v;
		}

		polynomial operator*(::std::uint32_t v) const noexcept {
			return polynomial{*this} *= v;
		}

		polynomial operator/(::std::uint32_t v) const noexcept {
			return polynomial{*this} /= v;
		}

		polynomial operator+(const polynomial& p) const noexcept {
			return polynomial{*this} += p;
		}

		polynomial operator-(const polynomial& p) const noexcept {
			return polynomial{*this} -= p;
		}

		polynomial operator*(polynomial&& p) const noexcept {
			return polynomial{*this} *= p;
		}

		polynomial operator*(const polynomial& p) const noexcept {
			return polynomial{*this} *= p;
		}
	};

	template<::std::uint32_t _p, ::std::uint32_t _g>
	polynomial<_p, _g> operator+(::std::uint32_t v, polynomial<_p, _g> p) noexcept {
		return p += v;
	}

	template<::std::uint32_t _p, ::std::uint32_t _g>
	polynomial<_p, _g> operator-(::std::uint32_t v, polynomial<_p, _g> p) noexcept {
		return p.negate() -= v;
	}

	template<::std::uint32_t _p, ::std::uint32_t _g>
	polynomial<_p, _g> operator*(::std::uint32_t v, polynomial<_p, _g> p) noexcept {
		return p *= v;
	}

	template<::std::uint32_t _p, ::std::uint32_t _g>
	polynomial<_p, _g> hadamard_product(polynomial<_p, _g> p, const polynomial<_p, _g>& q) noexcept {
		return polynomial<_p, _g>::hadamard_product(p.begin(), p.size(), q.begin());
	}

	template<::std::uint32_t _p, ::std::uint32_t _g>
	polynomial<_p, _g> inverse(polynomial<_p, _g> p) noexcept {
		return polynomial<_p, _g>::inverse(p.begin(), p.size());
	}

}