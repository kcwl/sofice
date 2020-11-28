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
		std::string generate_create_database_sql(const std::string& dbname, const std::string& charset, const std::string& collate)
		{
			return "create database if not exists " + dbname + " default character set " + charset + " collate " + collate + ";";
		}

		template<class T>
		constexpr auto generate_create_table_sql(const T& val, const std::string& engine, const std::string& charset, const std::string& primary_key = "")
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
		auto generate_delete_table_sql()
		{
			std::string sql{};

			sql.append("delete table if exists ");
			sql.append(aquarius::tuple_name<T>());
			sql.append(";");

			return sql;
		}

		template<class T>
		auto generate_sql(const T& val, insert_mode)
		{
			std::string sql{};

			auto table_name = std::string(aquarius::tuple_name<T>());

			sql.append("insert into " + table_name + " values(");

			auto func = [&sql](const std::string& element_column_name, auto element_column_val)
			{

				if constexpr(is_class_v<decltype(element_column_val)>)
					sql.append("'");

				//if constexpr (is_string<decltype(element_column_val)>::value)
				//sql.append(detail::to_string(element_column_val));
				//aquarius::iostream ios;

				//ios << element_column_val;
				//sql.append(ios.to_json_value<decltype(element_column_val)>());

				if constexpr(is_class_v<decltype(element_column_val)>)
					sql.append("'");

				sql.append(",");
			};

			detail::for_each(val, func);

			//sql.erase(sql.end() - 1);

			//sql.append(");");
			return sql.replace(sql.size() - 1, 1, ");");
		}

		template<class T>
		auto generate_sql(const T& val, update_mode)
		{
			std::string sql{};

			auto table_name = std::string(aquarius::tuple_name<T>());

			sql.append("update " + table_name + " set ");

			auto func = [&sql](const std::string& element_column_name, auto element_column_val)
			{
				sql.append(element_column_name + " = ");
				if constexpr(is_string<decltype(element_column_val)>::value)
					sql.append("'");

				sql.append(detail::to_string(element_column_val));

				if constexpr(sofice::is_string<decltype(element_column_val)>::value)
					sql.append("'");

				sql.append(",");
			};

			detail::for_each(val, func);

			sql.erase(sql.end() - 1);

			sql.append("where id = " + std::to_string(val.id_) + ";");

			//sql.append(";");
			return sql;
		}

		template<class T>
		auto generate_sql(const T& val, select_mode)
		{
			std::string sql{};

			auto table_name = std::string(aquarius::tuple_name<T>());

			return sql.append("select * from " + table_name + ";");
		}

		template<class T>
		auto generate_sql(const T& val, delete_mode)
		{
			std::string sql{};

			auto table_name = std::string(aquarius::tuple_name<T>());

			return sql.append("delete from " + table_name + ";");
		}

		//template<class T>
		//auto generate_sql(const T& val, insert_mode, const std::string& condition = "")
		//{
		//	std::string sql{};

		//	auto table_name = std::string(aquarius::tuple_name<T>());

		//	if constexpr(std::is_same_v<Mode, insert_mode>)
		//	{
		//		sql.append("insert into " + table_name + " values(");

		//		auto func = [&sql](const std::string& element_column_name, auto element_column_val)
		//		{

		//			if constexpr(is_string<decltype(element_column_val)>::value)
		//				sql.append("'");

		//			sql.append(detail::to_string(element_column_val));

		//			if constexpr(is_string<decltype(element_column_val)>::value)
		//				sql.append("'");

		//			sql.append(",");
		//		};

		//		detail::for_each(val,func);

		//		sql.erase(sql.end() - 1);

		//		sql.append(");");
		//	}
		//	else if constexpr(std::is_same_v<Mode, update_mode>)
		//	{
		//		sql.append("update " + table_name + " set ");

		//		auto func = [&sql](const std::string& element_column_name, auto element_column_val)
		//		{
		//			sql.append(element_column_name + " = ");
		//			if constexpr(is_string<decltype(element_column_val)>::value)
		//				sql.append("'");

		//			sql.append(detail::to_string(element_column_val));

		//			if constexpr(aquarius::is_string<decltype(element_column_val)>::value)
		//				sql.append("'");

		//			sql.append(",");
		//		};

		//		detail::for_each(val, func);

		//		sql.erase(sql.end() - 1);

		//		if(!condition.empty())
		//			sql.append(" where " + condition);

		//		sql.append(";");
		//	}
		//	else if constexpr(std::is_same_v<Mode, select_mode>)
		//	{
		//		sql.append("select * from " + table_name);

		//		if(!condition.empty())
		//			sql.append(" where " + condition);

		//		sql.append(";");
		//	}
		//	else if constexpr(std::is_same_v<Mode, delete_mode>)
		//	{
		//		sql.append("delete from " + table_name);

		//		if(!condition.empty())
		//			sql.append(" where " + condition);

		//		sql.append(";");
		//	}

		//	return sql;
		//}
	}
}