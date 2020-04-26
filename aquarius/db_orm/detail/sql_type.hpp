#pragma once
#include <string>

namespace aquarius
{
	namespace mysql_type
	{
		template<class T>
		struct indentify {};

		constexpr auto mysql_type(indentify<std::string>)
		{
			return "varchar(255)";
		}

		constexpr auto mysql_type(indentify<int>)
		{
			return "int(11)";
		}
	}
}