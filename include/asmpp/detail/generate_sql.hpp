#pragma once
#include <array>
#include <typeinfo>
#include <functional>
#include "sql_type.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"

#pragma warning(disable:4100)

namespace asmpp
{
	namespace detail
	{
		template<typename T>
		std::string create()
		{
			constexpr std::string_view table_name = reflect::rf_name<T>();

			std::string sql = "create table " + std::string(table_name) + " (";

			detail::for_each(T{}, [&sql] (auto name, auto value, std::size_t&& I)
				{
					sql.append("" + std::string(name) + " " + detail::mysql_type(detail::indentify<decltype(value)>{}));

					if (I != reflect::rf_size_v<T> -1)
						sql.append(",");
				});

			return sql;
		}

		template<typename T>
		std::string remove()
		{
			std::string sql = "delete table if exists ";

			constexpr auto table_name = reflect::rf_name<T>();

			sql.append(table_name);

			sql.append(";");

			return sql;
		}

		template<typename Mode, typename T>
		struct generate 
		{
			static std::string sql(T&& val)
			{
				return std::string{};
			}
		};

		template<typename T>
		struct generate<insert_mode,T>
		{
			static std::string sql(T const& val)
			{
				constexpr auto table_name = reflect::rf_name<T>();

				std::string sql = "insert into " + std::string(table_name) + " values(";

				detail::for_each(val, [&sql](auto&& value,std::size_t I)
					{
						sql += detail::to_string(std::move(value));

						if (I != reflect::rf_size_v<T> -1)
							sql.append(",");
					});

				sql.append(");");

				return sql;
			}
		};

		template<typename T>
		struct generate<update_mode, T>
		{
			static std::string sql(T&& val)
			{
				static_assert(reflect::rf_size_v<T> > 1, "T is not permited null");
				
				constexpr auto table_name = reflect::rf_name<std::remove_reference_t<T>>();

				std::string sql = "update " + std::string(table_name) + " set ";

				detail::for_each_elem(std::move(val), [&sql](auto name, auto value, std::size_t&& I)
					{
						sql.append(std::string(name) + " = ");

						sql.append(detail::to_string(std::move(value)));

						if(I != reflect::rf_size_v<T> -1)
							sql.append(",");
					});

				sql.append(" where " + std::string(reflect::rf_elem_name<T,0>()) + " = " + std::to_string(reflect::rf_element<0>(std::forward<T>(val))) + ";");

				return sql;
			}
		};

		template<typename T>
		struct generate<delete_mode,T>
		{
			static std::string sql()
			{
				auto table_name = std::string(reflect::rf_name<std::remove_reference_t<T>>());

				std::string sql = "delete from " + std::string(table_name) + ";";

				return sql;
			}
		};

		template<typename T>
		struct generate<select_mode, T>
		{
			static std::string sql()
			{
				auto table_name = std::string(reflect::rf_name<std::remove_reference_t<T>>());

				std::string sql = "select * from " + std::string(table_name) + ";";

				return sql;
			}
		};
	}
}