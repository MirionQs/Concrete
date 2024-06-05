#include <cstdint>
#include <limits>

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

	// Assuming the compiler is one of the MSVC, GCC and Clang
#if defined(_MSC_VER)
	using int128_t = ::std::_Signed128;
	using uint128_t = ::std::_Unsigned128;
#else
	using int128_t = __int128;
	using uint128_t = unsigned __int128;
#endif

	template<class T>
	constexpr bool is_integral_v = ::std::numeric_limits<T>::is_integer;

	template<class T>
	constexpr bool is_signed_v = ::std::numeric_limits<T>::is_signed;

	template<class T>
	constexpr bool is_unsigned_v = ::concrete::is_integral_v<T> && !::concrete::is_signed_v<T>;

	template<class T>
	struct is_integral : ::std::bool_constant<::concrete::is_integral_v<T>> {};

	template<class T>
	struct is_signed : ::std::bool_constant<::concrete::is_signed_v<T>> {};

	template<class T>
	struct is_unsigned : ::std::bool_constant<::concrete::is_unsigned_v<T>> {};

	template<class T>
	concept integral = ::concrete::is_integral_v<T>;

	template<class T>
	concept signed_integral = ::concrete::is_signed_v<T>;

	template<class T>
	concept unsigned_integral = ::concrete::is_unsigned_v<T>;

	template<bool is_signed, ::std::size_t size>
	struct integer {};

	template<>
	struct integer<true, 1> {
		using type = ::concrete::int8_t;
	};

	template<>
	struct integer<true, 2> {
		using type = ::concrete::int16_t;
	};

	template<>
	struct integer<true, 4> {
		using type = ::concrete::int32_t;
	};

	template<>
	struct integer<true, 8> {
		using type = ::concrete::int64_t;
	};

	template<>
	struct integer<true, 16> {
		using type = ::concrete::int128_t;
	};

	template<>
	struct integer<false, 1> {
		using type = ::concrete::uint8_t;
	};

	template<>
	struct integer<false, 2> {
		using type = ::concrete::uint16_t;
	};

	template<>
	struct integer<false, 4> {
		using type = ::concrete::uint32_t;
	};

	template<>
	struct integer<false, 8> {
		using type = ::concrete::uint64_t;
	};

	template<>
	struct integer<false, 16> {
		using type = ::concrete::uint128_t;
	};

	template<::std::size_t size>
	struct signed_integer {
		using type = ::concrete::integer<true, size>::type;
	};

	template<::std::size_t size>
	struct unsigned_integer {
		using type = ::concrete::integer<false, size>::type;
	};

	template<bool is_signed, ::std::size_t size>
	using integer_t = ::concrete::integer<is_signed, size>::type;

	template<::std::size_t size>
	using signed_integer_t = ::concrete::signed_integer<size>::type;

	template<::std::size_t size>
	using unsigned_integer_t = ::concrete::unsigned_integer<size>::type;

	template<::concrete::integral T>
	using make_signed_t = ::concrete::signed_integer_t<sizeof(T)>;

	template<::concrete::integral T>
	using make_unsigned_t = ::concrete::unsigned_integer_t<sizeof(T)>;

	template<class T>
	struct make_signed {
		using type = ::concrete::make_signed_t<T>;
	};

	template<class T>
	struct make_unsigned {
		using type = ::concrete::make_unsigned_t<T>;
	};

}