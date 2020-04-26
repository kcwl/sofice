#pragma once
#include <type_traits>
#include <string>

namespace aquarius
{
	struct insert_mode { };

	struct update_mode { };

	struct delete_mode { };

	struct select_mode { };

	struct create_mode { };


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
}




