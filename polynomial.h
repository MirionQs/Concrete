#pragma once

#include "int_m.h"

#include <vector>

namespace concrete {

	template<::concrete::integral T>
	class polynomial_arithmetic {
	public:
		using value_type = ::concrete::make_unsigned_t<T>;
		using modular_arithmetic_type = ::concrete::modular_arithmetic<value_type>;

	private:
		const modular_arithmetic_type _mod;
		const value_type _g;
		::std::vector<value_type> _roots;

		void _precomputeRoots(::std::size_t size) noexcept {
			::std::size_t oldSize{_roots.size()};
			if (size <= oldSize) {
				return;
			}

			_roots.resize(size);

			value_type m{_mod.modulo() >> (::std::countr_zero(oldSize) + 1)};
			for (::std::size_t half{oldSize}; half != size; half <<= 1, m >>= 1) {
				value_type r{_mod.power(_g, m)};
				for (::std::size_t i{half}; i != half << 1; i += 2) {
					_roots[i] = _roots[i >> 1];
					_roots[i + 1] = r * _roots[i];
				}
			}
		}

	public:
		explicit polynomial_arithmetic(value_type modulo, value_type g) noexcept :
			_mod{modulo},
			_g{g},
			_roots{1, 1} {}

		const modular_arithmetic_type& modular_arithmetic() const noexcept {
			return _mod;
		}

		template<::std::ranges::random_access_range R>
		void negate(R&& range) noexcept {
			for (value_type& i : range) {
				i = _mod.negate(i);
			}
		}

		template<::std::ranges::random_access_range R>
		void add(R&& range, value_type value) noexcept {
			value_type& i{*range.begin()};
			i = _mod.add(i, value);
		}

		template<::std::ranges::random_access_range R>
		void subtract(R&& range, value_type value) noexcept {
			value_type& i{*range.begin()};
			i = _mod.subtract(i, value);
		}

		template<::std::ranges::random_access_range R>
		void multiply(R&& range, value_type value) noexcept {
			for (value_type& i : range) {
				i = _mod.multiply(i, value);
			}
		}

		template<::std::ranges::random_access_range R>
		void divide(R&& range, value_type value) noexcept {
			value_type inverse{_mod.inverse(value)};
			for (value_type& i : range) {
				i = _mod.multiply(i, inverse);
			}
		}

		template<::std::ranges::random_access_range R, ::std::random_access_iterator iter>
		void add(R&& range, iter first) noexcept {
			for (value_type& i : range) {
				i = _mod.add(i, *first++);
			}
		}

		template<::std::ranges::random_access_range R, ::std::random_access_iterator iter>
		void subtract(R&& range, iter first) noexcept {
			for (value_type& i : range) {
				i = _mod.subtract(i, *first++);
			}
		}

		template<::std::ranges::random_access_range R, ::std::random_access_iterator iter>
		void multiply_inplace(R&& range, iter first) noexcept {
			fast_fourier_transform(range);
			fast_fourier_transform(::std::ranges::subrange{first, first + (range.end() - range.begin())});
			hadamard_product(range, first);
			inverse_fast_fourier_transform(range);
		}

		template<::std::ranges::random_access_range R, ::std::random_access_iterator iter>
		void hadamard_product(R&& range, iter first) noexcept {
			for (value_type& i : range) {
				i = _mod.multiply(i, *first++);
			}
		}

		template<::std::ranges::random_access_range R>
		void fast_fourier_transform(R&& range) noexcept {
			::std::size_t size{range.size()};
			if (size == 1) {
				return;
			}

			_precomputeRoots(size);

			auto x{range.begin()};
			for (::std::size_t half{size >> 1}; half != 1; half >>= 1) {
				for (::std::size_t i{0}; i != size; i += half << 1) {
					auto r{_roots.begin() + half};
					for (::std::size_t j{i}; j != i + half; ++j) {
						value_type p{x[j]}, q{x[j + half]};
						x[j] = _mod.add(p, q);
						x[j + half] = _mod.multiply(_mod.subtract(p, q), *r++);
					}
				}
			}

			for (::std::size_t i{0}; i != size; i += 2) {
				value_type p{x[i]}, q{x[i + 1]};
				x[i] = _mod.add(p, q);
				x[i + 1] = _mod.subtract(p, q);
			}
		}

		template<::std::ranges::random_access_range R>
		void inverse_fast_fourier_transform(R&& range) noexcept {
			::std::size_t size{range.size()};
			if (size == 1) {
				return;
			}

			_precomputeRoots(size);

			auto x{range.begin()};
			value_type m{_mod.modulo()};
			value_type nInverse{m - (m >> ::std::countr_zero(size))};
			for (::std::size_t i{0}; i != size; i += 2) {
				value_type p{x[i]}, q{x[i + 1]};
				x[i] = _mod.multiply(_mod.add(p, q), nInverse);
				x[i + 1] = _mod.multiply(_mod.subtract(p, q), nInverse);
			}

			for (::std::size_t half{2}; half != size; half <<= 1) {
				for (::std::size_t i{0}; i != size; i += half << 1) {
					value_type p{x[i]}, q{x[i + half]};
					x[i] = _mod.add(p, q);
					x[i + half] = _mod.subtract(p, q);
					auto r{_roots.begin() + (half << 1)};
					for (::std::size_t j{i + 1}; j != i + half; ++j) {
						value_type p{x[j]}, q{_mod.multiply(x[j + half], *--r)};
						x[j] = _mod.subtract(p, q);
						x[j + half] = _mod.add(p, q);
					}
				}
			}
		}
	};

	template<class T>
	explicit polynomial_arithmetic(T, T)->polynomial_arithmetic<T>;

}