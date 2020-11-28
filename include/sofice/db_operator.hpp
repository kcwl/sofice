#pragma once
#include <vector>
#include <iterator>
#include <functional>
#include <algorithm>

namespace sofice
{
	namespace linqcpp
	{
		template<class _Ty>
		class linq_data
		{
		public:
			linq_data(const _Ty& container)
				: _container(container)
			{
			}

		public:


			template<class Func>
			auto concat(const _Ty& data, Func&& func)
			{
				for(auto iter = _container.begin(); iter != _container.end();)
				{
					if(!func(*iter, data))
						iter = _container.erase(iter);
					else
						++iter;
				}

				return *this;
			}

			template<class Func>
			auto where(Func&& func) -> decltype(*this)
			{
				for(auto iter = _container.begin(); iter != _container.end();)
				{
					if(!func(*iter))
						iter = _container.erase(iter);
					else
						++iter;
				}

				return *this;
			}

			template<class Func>
			auto select(Func&& func)
			{
				return where(std::forward<Func>(func)).select();
			}

			auto select()
			{
				return _container;
			}
		private:
			_Ty _container;
		};

		template<class _Ty>
		auto from(const _Ty& data)
		{
			return linq_data(data);
		}
	}
}