#pragma once
#include <sstream>
#include <string>
#include <string_view>
#include <mysql.h>
#include "type_traits.hpp"
#include "../reflect/include/reflect.hpp"

namespace sofice
{
	namespace detail
	{
		template<typename Tuple,typename Func,std::size_t... I>
		constexpr auto for_each_impl(Tuple&& tuple, Func&& f, std::index_sequence<I...>)
		{
			return (std::forward<Func>(f)(reflect::rf_elem_name<Tuple,I>(), reflect::rf_element<I>(std::forward<Tuple>(tuple)),std::move(I)),...);
		}
		
		template<typename T,typename Func>
		constexpr auto for_each(T&& tp, Func&& f)
		{
			return detail::template for_each_impl(std::forward<T>(tp), std::forward<Func>(f), std::make_index_sequence<reflect::rf_size_v<T>>{});
		}

		template<typename T>
		std::string to_string(T&& val);

		template<std::size_t I,typename V>
		void f(std::stringstream& ss, V&& val)
		{
			if(I == std::forward<V>(val).index())
				ss << detail::template to_string(std::get<I>(std::forward<V>(val)));
		}


		template<typename V, std::size_t... I>
		constexpr auto for_each_variant(V&& val, std::stringstream& ss, std::index_sequence<I...>)
		{
			return (detail::template f<I>(ss, std::forward<V>(val)),...);
		}

		template<typename V>
		constexpr auto for_each_variant(V&& val,std::stringstream& ss)
		{
			return for_each_variant(std::forward<V>(val), ss, std::make_index_sequence<std::variant_size_v<std::remove_cvref_t<V>>>{});
		}


		template<typename T>
		std::string to_string(T&& val)
		{
			std::stringstream ss;
			if constexpr (is_string_v<std::remove_cvref_t<T>>)
			{
				ss << "'" << val << "'";
			}
			else if constexpr(is_container_v<std::remove_cvref_t<T>>)
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
				ss << val;
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

		template<typename T>
		auto cast(const char* val)
		{
			std::stringstream ss;
			ss << val;
			
			T t;
			ss >> t;

			return t;
		}

		template<typename T,std::size_t... I>
		auto to_struct_impl(const MYSQL_ROW& row, std::index_sequence<I...>)
		{
			return T{ cast<decltype(reflect::rf_element<I>(T{}))>(row[I])... };
		}

		template<typename T>
		auto to_struct(const MYSQL_ROW& row)
		{
			return detail::template to_struct_impl<T>(row, std::make_index_sequence<reflect::rf_size_v<T>>{});
		}

		template<const std::string_view&... strs>
		struct concat
		{
			constexpr static auto impl() noexcept
			{
				constexpr auto len = (strs.size() + ... + 0);
				std::array<char, len + 1> arr{};

				auto f = [i = 0, &arr](auto const& str) mutable
				{
					for (auto s : str)
						arr[i++] = s;

					return arr;
				};

				(f(strs), ...);

				arr[len] = '\0';

				return arr;
			}

			static constexpr auto arr = impl();

			static constexpr std::string_view value{arr.data(), arr.size() - 1};
		};

		template<std::string_view const&... strs>
		constexpr static auto concat_v = concat<strs...>::value;
	}
}