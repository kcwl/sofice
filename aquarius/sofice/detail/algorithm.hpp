#pragma once
#include <sstream>
#include <string_view>
#include "utility.hpp"

namespace aquarius
{
	namespace detail
	{
		template<class Tuple,class Func,std::size_t... I>
		constexpr auto for_each(Tuple&& tuple, Func&& f, std::index_sequence<I...>)
		{
			return (std::forward<Func>(f)(aquarius::tuple_element_name<I>(std::forward<Tuple>(tuple)), aquarius::tuple_element<I>(std::forward<Tuple>(tuple))),...);
		}
		
		template<class T,class Func>
		constexpr auto for_each(T&& tp, Func&& f)
		{
			return for_each(std::forward<T>(tp), std::forward<Func>(f), std::make_index_sequence<aquarius::tuple_size_v<T>>{});
		}

		template<class T>
		constexpr auto to_string(T&& val);

		template<std::size_t I,class V>
		void f(std::stringstream& ss, V&& val)
		{
			if(I == std::forward<V>(val).index())
				ss << detail::to_string(std::get<I>(std::forward<V>(val)));
		}


		template<class V, std::size_t... I>
		constexpr auto for_each_variant(V&& val, std::stringstream& ss, std::index_sequence<I...>)
		{
			return (f<I>(ss, std::forward<V>(val)),...);
		}

		template<class V>
		constexpr auto for_each_variant(V&& val,std::stringstream& ss)
		{
			return for_each_variant(std::forward<V>(val), ss, std::make_index_sequence<std::variant_size_v<std::remove_cvref_t<V>>>{});
		}


		template<class T>
		constexpr auto to_string(T&& val)
		{
			std::stringstream ss;
			if constexpr(is_container_v<std::remove_cvref_t<T>>)
			{
				ss << "{";
				std::for_each(val.begin(), val.end(), [&](auto iter)
					{
						if constexpr (is_byte_v<decltype(iter)>)
							ss << static_cast<char>(iter) << ",";
						else
						{
							ss << "{";
							
							for_each(val, [&ss](const std::string& name, auto value)
								{
									ss << value << ",";
								});

							auto result = ss.str();
							result.erase(result.size() - 1);
							result.append("},");
						}
					});

				auto result = ss.str();
				result.erase(result.size() - 1);
				result.append("}");

				return result;
			}
			else if constexpr(is_byte_v<std::remove_cvref_t<T>>)
			{
				ss << static_cast<char>(std::forward<T>(val));
			}
			else if constexpr (std::is_trivial_v<std::remove_reference_t<T>>)
			{
				
			}
			else if constexpr (is_variant_v<std::remove_cvref_t<T>>)
			{
				//constexpr std::size_t index = std::forward<T>(val).index();
				//ss << detail::to_string(std::get<index>(std::forward<T>(val)));
				/*auto result = *///for_each_variant(std::forward<T>(val),ss);

				/*ss << detail::to_string(std::get<decltype(result)>(std::forward<T>(val)));*/
			}
			else
			{
				ss << val;
			}

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

		//template<class T,std::size_t... I>
		//auto construct_data(const MYSQL_ROW& row, std::index_sequence<I...>)
		//{
		//	return T{ cast<decltype(aquarius::tuple_element<I>(T{}))>(row[I])... };
		//}

		//template<class T>
		//auto construct_data(const MYSQL_ROW& row)
		//{
		//	return construct_data<T>(row, std::make_index_sequence<aquarius::tuple_size_v<T>>{});
		//}

		template<std::string_view const&, class, std::string_view const&, class>
		struct combine;

		template<std::string_view const& lhs,std::size_t... L, std::string_view const& rhs, std::size_t... R>
		struct combine<lhs,std::index_sequence<L...>,rhs,std::index_sequence<R...>>
		{
			inline static constexpr const char value[]{lhs[L]...,rhs[R]...,0};
		};

		template<std::string_view const&...> 
		struct concat;

		template<>
		struct concat<>
		{
			static constexpr std::string_view value = "";
		};

		template<std::string_view const& s1, std::string_view const& s2>
		struct concat<s1,s2>
		{
			static constexpr std::string_view value = combine<s1, std::make_index_sequence<s1.size()>, s2, std::make_index_sequence<s2.size()>>::value;
		};

		template<std::string_view const& S, std::string_view const&... args>
		struct concat<S, args...>
		{
			static constexpr std::string_view value = concat<S, concat<args...>::value>::value;
		};

		template<std::string_view const&... args>
		inline static constexpr auto concat_v = concat<args...>::value;
	}
}