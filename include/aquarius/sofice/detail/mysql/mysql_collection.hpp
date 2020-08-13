#pragma once
#include "../generate_sql.hpp"


namespace aquarius
{
	namespace detail
	{
		namespace mysqld
		{
			class mysql_collection
			{
			public:
				mysql_collection() = default;

			public:
				template<class T>
				auto insert(T&& val)
				{
					return detail::generate_sql(std::forward<T>(val), insert_mode{});
				}

				template<class T>
				auto update(T&& val)
				{
					return detail::generate_sql(std::forward<T>(val), update_mode{});
				}

				template<class T>
				auto select(const std::string& condition)
				{
					return detail::generate_sql(T{}, select_mode{});
				}

				template<class T>
				auto remove(const std::string& condition)
				{
					return detail::generate_sql(T{}, select_mode{});
				}
			};
		}
	}
}