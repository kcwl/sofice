#pragma once
#include <array>
#include <typeinfo>
#include <functional>
#include "sql_type.hpp"
#include "type_traits.hpp"
#include "algorithm.hpp"
#include <reflect.hpp>
namespace sofice
{
	namespace detail
	{
		constexpr std::string_view create_table = "create table ";

		template<class T>
		constexpr auto create_table_sql(const T& val, const std::string& engine, const std::string& charset, const std::string& primary_key = "")
		{
			std::string sql{};
			std::string table_name = std::string(aquarius::tuple_name<T>());
			sql.append("create table " + table_name + " (");

			auto func = [&sql,primary_key](const std::string& element_column_name, auto element_column_val)
			{
				std::string is_null{};

				element_column_name.compare(primary_key) == 0 ? is_null = " not null" : is_null = " default null";

				sql.append("" + element_column_name + " " + detail::mysql_type(detail::indentify<decltype(element_column_val)>{}) + is_null + ",");
			};

			detail::for_each(val, func);

			!primary_key.empty() ? (void)sql.append("primary key (" + primary_key + ")") : (void)sql.erase(sql.end() - 1);

			sql.append(") engine=" + engine + " default charset=" + charset + ";");

			return sql;
		}

		template<class T>
		auto delete_table_sql()
		{
			std::string sql{};

			sql.append("delete table if exists ");
			sql.append(aquarius::tuple_name<T>());
			sql.append(";");

			return sql;
		}

		template<class Mode, class T>
		auto generate_sql(const T& val)
		{
			std::string sql{};

			if constexpr (std::is_same_v<Mode, insert_mode>)
			{
				auto table_name = std::string(aquarius::tuple_name<T>());

				sql.append("insert into " + table_name + " values(");

				detail::for_each(val, [&sql] (auto item_name , auto value) 
					{
						item_name = "";
						sql += value + ",";
					});

				sql.replace(sql.size() - 1, 1, ");");
			}
			else if constexpr (std::is_same_v<Mode, update_mode>)
			{
				auto table_name = std::string(aquarius::tuple_name<T>());

				sql.append("update " + table_name + " set ");

				detail::for_each(val, [&sql] (const std::string& element_column_name, auto element_column_val)
					{
						sql.append(element_column_name + " = ");

						sql.append(detail::to_string(element_column_val));

						sql.append(",");
					});

				sql.erase(sql.end() - 1);

				sql.append("where id = " + std::to_string(val.id_) + ";");
			}
			else if constexpr (std::is_same_v<Mode, delete_mode>)
			{
				auto table_name = std::string(aquarius::tuple_name<T>());

				sql.append("delete from " + table_name + ";");
			}
			else if constexpr (std::is_same_v<Mode, select_mode>)
			{
				auto table_name = std::string(aquarius::tuple_name<T>());

				sql.append("select * from " + table_name + ";");
			}

			return sql;
		}
	}
}