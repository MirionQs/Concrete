#pragma once

#include "int_mod.h"

#include <vector>

namespace concrete {

	template<::concrete::uint32_t m, ::concrete::uint32_t g>
	class polynomial : public ::std::vector<::concrete::int_mod<m>> {
	public:
		using value_type = ::std::vector<int_mod<m>>::value_type;
		using iterator = ::std::vector<int_mod<m>>::iterator;
		using const_iterator = ::std::vector<int_mod<m>>::const_iterator;

	private:
		static inline ::std::vector<value_type> _roots{1, 1};

		static void _precomputeRoots(::std::size_t n) noexcept {
			static constexpr value_type gInverse{::concrete::inverse(value_type{g})};
			::std::size_t oldSize{_roots.size()};
			if (n <= oldSize) {
				return;
			}
			_roots.resize(n);
			::concrete::uint32_t t{m >> ::std::countr_zero(oldSize) >> 1};
			for (::std::size_t half{oldSize}; half != n; half <<= 1, t >>= 1) {
				value_type r{power(gInverse, t)};
				for (::std::size_t i{half}; i != half << 1; i += 2) {
					_roots[i] = _roots[i >> 1];
					_roots[i + 1] = r * _roots[i];
				}
			}
		}

	public:
		static void fast_fourier_transform(iterator x, ::std::size_t n) noexcept {
			if (n == 1) {
				return;
			}
			_precomputeRoots(n);
			for (::std::size_t half{n >> 1}; half != 1; half >>= 1) {
				for (::std::size_t i{0}; i != n; i += half << 1) {
					auto r{_roots.begin() + half};
					for (::std::size_t j{i}; j != i + half; ++j) {
						value_type p{x[j]}, q{x[j + half]};
						x[j] = p + q;
						x[j + half] = (p - q) * *r++;
					}
				}
			}
			for (::std::size_t i{0}; i != n; i += 2) {
				value_type p{x[i]}, q{x[i + 1]};
				x[i] = p + q;
				x[i + 1] = p - q;
			}
		}

		static void inverse_fast_fourier_transform(iterator x, ::std::size_t n) noexcept {
			if (n == 1) {
				return;
			}
			_precomputeRoots(n);
			value_type nInverse{m - (m >> ::std::countr_zero(n))};
			for (::std::size_t i{0}; i != n; i += 2) {
				value_type p{x[i]}, q{x[i + 1]};
				x[i] = (p + q) * nInverse;
				x[i + 1] = (p - q) * nInverse;
			}
			for (::std::size_t half{2}; half != n; half <<= 1) {
				for (::std::size_t i{0}; i != n; i += half << 1) {
					value_type p{x[i]}, q{x[i + half]};
					x[i] = p + q;
					x[i + half] = p - q;
					auto r{_roots.begin() + (half << 1)};
					for (::std::size_t j{i + 1}; j != i + half; ++j) {
						value_type p{x[j]}, q{x[j + half] * *--r};
						x[j] = p - q;
						x[j + half] = p + q;
					}
				}
			}
		}

		static void negate(iterator x, ::std::size_t n) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i].negate();
			}
		}

		static void add(iterator x, ::std::size_t n, value_type v) noexcept {
			x[0] += v;
		}

		static void subtract(iterator x, ::std::size_t n, value_type v) noexcept {
			x[0] -= v;
		}

		static void multiply(iterator x, ::std::size_t n, value_type v) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= v;
			}
		}

		static void divide(iterator x, ::std::size_t n, value_type v) noexcept {
			value_type vInverse{inverse(v)};
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= vInverse;
			}
		}

		static void add(iterator x, ::std::size_t n, const_iterator y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] += y[i];
			}
		}

		static void subtract(iterator x, ::std::size_t n, const_iterator y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] -= y[i];
			}
		}

		static void multiply(iterator x, ::std::size_t n, iterator y) noexcept {
			fast_fourier_transform(x, n);
			fast_fourier_transform(y, n);
			hadamard_product(x, n, y);
			inverse_fast_fourier_transform(x, n);
		}

		static void hadamard_product(iterator x, ::std::size_t n, const_iterator y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= y[i];
			}
		}

		static void inverse(iterator x, ::std::size_t n) noexcept {

		}

		using ::std::vector<value_type>::vector;

		polynomial& negate() noexcept {
			negate(this->begin(), this->size());
			return *this;
		}

		polynomial& operator+=(::concrete::uint32_t v) noexcept {
			add(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator-=(::concrete::uint32_t v) noexcept {
			subtract(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator*=(::concrete::uint32_t v) noexcept {
			multiply(this->begin(), this->size(), v);
			return *this;
		}

		polynomial& operator/=(::concrete::uint32_t v) noexcept {
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

		polynomial operator+(::concrete::uint32_t v) const noexcept {
			return polynomial{*this} += v;
		}

		polynomial operator-(::concrete::uint32_t v) const noexcept {
			return polynomial{*this} -= v;
		}

		polynomial operator*(::concrete::uint32_t v) const noexcept {
			return polynomial{*this} *= v;
		}

		polynomial operator/(::concrete::uint32_t v) const noexcept {
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

	template<auto m, auto g>
	polynomial<m, g> operator+(::concrete::uint32_t v, polynomial<m, g> p) noexcept {
		return p += v;
	}

	template<auto m, auto g>
	polynomial<m, g> operator-(::concrete::uint32_t v, polynomial<m, g> p) noexcept {
		return p.negate() -= v;
	}

	template<auto m, auto g>
	polynomial<m, g> operator*(::concrete::uint32_t v, polynomial<m, g> p) noexcept {
		return p *= v;
	}

	template<auto m, auto g>
	polynomial<m, g> hadamard_product(polynomial<m, g> p, const polynomial<m, g>& q) noexcept {
		return polynomial<m, g>::hadamard_product(p.begin(), p.size(), q.begin());
	}

	template<auto m, auto g>
	polynomial<m, g> inverse(polynomial<m, g> p) noexcept {
		return polynomial<m, g>::inverse(p.begin(), p.size());
	}

}