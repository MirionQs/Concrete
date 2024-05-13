#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

#if defined(_MSC_VER)
#include <__msvc_int128.hpp>
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
	using ::std::size_t;
	using ::std::ptrdiff_t;

	// Assuming the compiler is one of the MSVC, GCC and Clang
#if defined(_MSC_VER)
	using int128_t = ::std::_Signed128;
	using uint128_t = ::std::_Unsigned128;
#else
	using int128_t = __int128;
	using uint128_t = unsigned __int128;
#endif

	template<class T>
	using double_size_t =
		::std::conditional_t<::std::is_same_v<T, int8_t>, int16_t,
		::std::conditional_t<::std::is_same_v<T, int16_t>, int32_t,
		::std::conditional_t<::std::is_same_v<T, int32_t>, int64_t,
		::std::conditional_t<::std::is_same_v<T, int64_t>, int128_t,
		::std::conditional_t<::std::is_same_v<T, uint8_t>, uint16_t,
		::std::conditional_t<::std::is_same_v<T, uint16_t>, uint32_t,
		::std::conditional_t<::std::is_same_v<T, uint32_t>, uint64_t,
		::std::conditional_t<::std::is_same_v<T, uint64_t>, uint128_t,
		void>>>>>>>>;

	template<class T>
	using half_size_t =
		::std::conditional_t<::std::is_same_v<T, int16_t>, int8_t,
		::std::conditional_t<::std::is_same_v<T, int32_t>, int16_t,
		::std::conditional_t<::std::is_same_v<T, int64_t>, int32_t,
		::std::conditional_t<::std::is_same_v<T, int128_t>, int64_t,
		::std::conditional_t<::std::is_same_v<T, uint16_t>, uint8_t,
		::std::conditional_t<::std::is_same_v<T, uint32_t>, uint16_t,
		::std::conditional_t<::std::is_same_v<T, uint64_t>, uint32_t,
		::std::conditional_t<::std::is_same_v<T, uint128_t>, uint64_t,
		void>>>>>>>>;

}