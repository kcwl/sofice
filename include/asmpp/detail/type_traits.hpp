#pragma once
#include <type_traits>
#include <variant>
#include <string>

namespace asmpp
{
	struct insert_mode { };

	struct update_mode { };

	struct delete_mode { };

	struct select_mode { };

	struct create_mode { };

	struct remove_mode { };


	template<class T>
	struct is_string : std::false_type {};

	template<>
	struct is_string<char*> : std::true_type {};

	template<>
	struct is_string<const char*> : std::true_type {};

	template<>
	struct is_string<std::string_view> : std::true_type {};

	template<>
	struct is_string<std::string> : std::true_type {};

	template<typename T>
	static constexpr bool is_string_v = is_string<T>::value;

	template<class T,class = std::void_t<>>
	struct is_container : std::false_type { };

	template<class T>
	struct is_container<T, std::void_t<typename T::iterator>> : std::true_type { };

	template<class T>
	static constexpr bool is_container_v = is_container<T>::value;

	template<class T>
	struct is_byte : std::false_type { };

	template<>
	struct is_byte<std::byte> : std::true_type { };

	template<class T>
	static constexpr bool is_byte_v = is_byte<T>::value;

	template<class T>
	struct is_variant : std::false_type { };

	template<class... Args>
	struct is_variant<std::variant<Args...>> : std::true_type{ };

	template<class T>
	static constexpr bool is_variant_v = is_variant<T>::value;

}




