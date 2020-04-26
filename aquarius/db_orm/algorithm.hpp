#pragma once
#include <sstream>
#include "detail/utility.hpp"

namespace aquarius
{
	namespace algorithm
	{
		template<class Tuple,class Func,std::size_t... I>
		constexpr auto for_each(Tuple&& tuple, std::string& sql, Func&& f, std::index_sequence<I...>)
		{
			return (std::forward<Func>(f)(sql, aquarius::tuple_element_name<I>(std::forward<Tuple>(tuple)), aquarius::tuple_element<I>(std::forward<Tuple>(tuple))),...);
		}
		
		template<class T,class Func>
		constexpr auto for_each(T&& tp, std::string& sql, Func&& f)
		{
			return for_each(std::forward<T>(tp), sql, std::forward<Func>(f), std::make_index_sequence<aquarius::tuple_size_v<T>>{});
		}

		template<class T>
		constexpr auto to_string(T&& t)
		{
			std::stringstream ss;
			ss << t;

			return ss.str();
		}

		template<class T>
		auto cast(const char* val)
		{
			std::stringstream ss;
			ss << val;
			
			T t;
			ss >> t;

			return t;
		}

		template<class T,std::size_t... I>
		auto construct_data(const MYSQL_ROW& row, std::index_sequence<I...>)
		{
			return T{ cast<decltype(aquarius::tuple_element<I>(T{}))>(row[I])... };
		}

		template<class T>
		auto construct_data(const MYSQL_ROW& row)
		{
			return construct_data<T>(row, std::make_index_sequence<aquarius::tuple_size_v<T>>{});
		}
	}
}