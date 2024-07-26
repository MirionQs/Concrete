#pragma once

#include <cstdint>
#include <climits>
#include <limits>
#include <type_traits>

#ifdef _MSC_VER
#include <__msvc_int128.hpp>
#endif

#if CHAR_BIT != 8
#error "Non-8-bit bytes are not supported."
#endif

namespace concrete {

	using ::std::int8_t;
	using ::std::int16_t;
	using ::std::int32_t;
	using ::std::int64_t;

	using ::std::uint8_t;
	using ::std::uint16_t;
	using ::std::uint32_t;
	using ::std::uint64_t;

#ifdef _MSC_VER
	using int128_t = ::std::_Signed128;
	using uint128_t = ::std::_Unsigned128;
#else
	using int128_t = __int128;
	using uint128_t = unsigned __int128;
#endif

	template<bool is_signed, ::std::size_t size>
	struct integral {};

	template<>
	struct integral<true, 1> {
		using type = ::concrete::int8_t;
	};

	template<>
	struct integral<true, 2> {
		using type = ::concrete::int16_t;
	};

	template<>
	struct integral<true, 4> {
		using type = ::concrete::int32_t;
	};

	template<>
	struct integral<true, 8> {
		using type = ::concrete::int64_t;
	};

	template<>
	struct integral<true, 16> {
		using type = ::concrete::int128_t;
	};

	template<>
	struct integral<false, 1> {
		using type = ::concrete::uint8_t;
	};

	template<>
	struct integral<false, 2> {
		using type = ::concrete::uint16_t;
	};

	template<>
	struct integral<false, 4> {
		using type = ::concrete::uint32_t;
	};

	template<>
	struct integral<false, 8> {
		using type = ::concrete::uint64_t;
	};

	template<>
	struct integral<false, 16> {
		using type = ::concrete::uint128_t;
	};

	template<::std::size_t size>
	using signed_integral = ::concrete::integral<true, size>;

	template<::std::size_t size>
	using unsigned_integral = ::concrete::integral<false, size>;

	template<bool is_signed, ::std::size_t size>
	using integral_t = typename ::concrete::integral<is_signed, size>::type;

	template<::std::size_t size>
	using signed_integral_t = ::concrete::integral_t<true, size>;

	template<::std::size_t size>
	using unsigned_integral_t = ::concrete::integral_t<false, size>;

	template<class T>
	constexpr bool is_integral_v{::std::numeric_limits<T>::is_integer};

	template<class T>
	constexpr bool is_signed_v{::std::numeric_limits<T>::is_signed};

	template<class T>
	constexpr bool is_unsigned_v{::concrete::is_integral_v<T> && !::concrete::is_signed_v<T>};

	template<class T>
	using is_integral = ::std::bool_constant<::concrete::is_integral_v<T>>;

	template<class T>
	using is_signed = ::std::bool_constant<::concrete::is_signed_v<T>>;

	template<class T>
	using is_unsigned = ::std::bool_constant<::concrete::is_unsigned_v<T>>;

	template<class T>
	struct make_signed {
		static_assert(::concrete::is_integral_v<T>, "T should be an integral type.");

		using type = ::concrete::signed_integral_t<sizeof(T)>;
	};

	template<class T>
	struct make_unsigned {
		static_assert(::concrete::is_integral_v<T>, "T should be an integral type.");

		using type = ::concrete::unsigned_integral_t<sizeof(T)>;
	};

	template<class T>
	using make_signed_t = typename ::concrete::make_signed<T>::type;

	template<class T>
	using make_unsigned_t = typename ::concrete::make_unsigned<T>::type;

}