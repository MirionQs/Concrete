#pragma once

#include "int_p.h"

#include <vector>
#include <bit>

namespace concrete {

	template<::std::uint32_t _p = 998244353, ::std::uint32_t _g = 3>
	class polynomial : public ::std::vector<::concrete::int_p<_p>> {
		using _parent = ::std::vector<::concrete::int_p<_p>>;
		using _value = _parent::value_type;
		using _iter = _parent::iterator;
		using _constIter = _parent::const_iterator;

		static constexpr _value _gInverse{::concrete::inverse(_value{_g})};

		static inline ::std::vector<_value> _roots{1, 1};

		static void _precomputeRoots(::std::size_t n) noexcept {
			::std::size_t oldSize{_roots.size()};
			if (n <= oldSize) {
				return;
			}
			_roots.resize(n);
			::std::uint32_t m{(_p - 1) >> (::std::countr_zero(oldSize) + 1)};
			for (::std::size_t half{2}; half != n; half <<= 1, m >>= 1) {
				_value r{::concrete::power(_gInverse, m)};
				for (::std::size_t i{half}; i != half << 1; i += 2) {
					_roots[i] = _roots[i >> 1];
					_roots[i + 1] = r * _roots[i];
				}
			}
		}

		void _fastFourierTransform(_iter x, ::std::size_t n) noexcept {
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

		void _inverseFastFourierTransform(_iter x, ::std::size_t n) noexcept {
			_value nInverse{_p - ((_p - 1) >> ::std::countr_zero(n))};
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

	public:
		using _parent::_parent;

		void negate(_iter x, ::std::size_t n) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i].negate();
			}
		}

		void add(_iter x, ::std::size_t n, _value v) noexcept {
			x[0] += v;
		}

		void subtract(_iter x, ::std::size_t n, _value v) noexcept {
			x[0] -= v;
		}

		void multiply(_iter x, ::std::size_t n, _value v) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= v;
			}
		}

		void divide(_iter x, ::std::size_t n, _value v) noexcept {
			_value vInverse{::concrete::inverse(v)};
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= vInverse;
			}
		}

		void add(_iter x, ::std::size_t n, _constIter y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] += y[i];
			}
		}

		void subtract(_iter x, ::std::size_t n, _constIter y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] -= y[i];
			}
		}

		void multiply(_iter x, ::std::size_t n, _iter y) noexcept {
			_precomputeRoots(n);
			_fastFourierTransform(x, n);
			_fastFourierTransform(y, n);
			hadamard_product(x, n, y);
			_inverseFastFourierTransform(x, n);
		}

		void hadamard_product(_iter x, ::std::size_t n, _constIter y) noexcept {
			for (::std::size_t i{0}; i != n; ++i) {
				x[i] *= y[i];
			}
		}

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

		polynomial& hadamard_product(const polynomial& p) noexcept {
			hadamard_product(this->begin(), this->size(), p.begin());
			return *this;
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
	polynomial<_p, _g> hadamard_product(const polynomial<_p, _g>& p, const polynomial<_p, _g>& q) noexcept {
		return polynomial<_p, _g>{p}.hadamard_product(q);
	}

}