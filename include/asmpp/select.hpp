#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

namespace asmpp
{
	class select_context
	{
	public:
		explicit select_context(std::shared_ptr<service> service_ptr)
			: service_ptr_(service_ptr)
		{
		}

	public:
		template<typename T, typename Func>
		void excute(Func&& f,std::string condition)
		{
			auto sql = asmpp::detail::template generate<asmpp::select_mode, T>::sql(condition);

			auto results = service_ptr_->real_query<T>(sql);

			if(results.empty())
				return;

			f(results);
		}

		template<typename T>
		std::vector<T> query(std::string sql)
		{
			return service_ptr_->real_query<T>(sql);
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}