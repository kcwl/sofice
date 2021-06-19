#pragma once
#include <array>
#include <typeinfo>
#include <functional>
#include "sql_type.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"

#pragma warning(disable:4100)

namespace sofice
{
	namespace detail
	{
		template<typename T>
		std::string create(const std::string& primary_key, std::string const& engine, std::string const& charset)
		{
			constexpr std::string_view table_name = reflect::rf_name<T>();

			std::string sql = "create table " + std::string(table_name) + " (";

			detail::for_each(T{}, [&sql, primary_key] (auto name, auto value, std::size_t&& I)
				{
					std::string is_null = name.compare(primary_key) == 0 ? " not null" : " default null";

					sql.append("" + std::string(name) + " " + detail::mysql_type(detail::indentify<decltype(value)>{}) + is_null);

					if (I != reflect::rf_size_v<T> -1)
						sql.append(",");
				});

			!primary_key.empty() ? (void)sql.append(" primary key (" + primary_key + ")") : (void)sql.erase(sql.end() - 1);

			sql.append(") engine=" + engine + " default charset=" + charset + ";");

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

				detail::for_each(std::move(val), [&sql] (auto name, auto value, std::size_t&& I)					
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
			static std::string sql(T const& val)
			{
				constexpr auto table_name = reflect::rf_name<std::remove_reference_t<T>>();

				std::string sql = "update " + std::string(table_name) + " set ";

				detail::for_each(std::move(val), [&sql](auto name, auto value, std::size_t&& I)
					{
						sql.append(std::string(name) + " = ");

						sql.append(detail::to_string(std::move(value)));

						if(I != reflect::rf_size_v<T> -1)
							sql.append(",");
					});

				sql.append(" where id = " + std::to_string(val.id) + ";");

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