#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

namespace asmpp
{
	class ctx_select
	{
	public:
		explicit ctx_select(std::shared_ptr<service> service_ptr)
			: service_ptr_(service_ptr)
		{
		}

	public:
		template<typename T, typename Func>
		void query_if(const std::string& condition, Func&& f)
		{
			auto sql = asmpp::detail::template generate<asmpp::select_mode, T>::sql(condition);

			error_code ec;

			auto results = service_ptr_->real_query<T>(sql,ec);

			f(ec,results);
		}

		template<typename T,typename Func>
		void query(const std::string& sql,Func&& f)
		{
			error_code ec;
			auto results = service_ptr_->real_query<T>(sql,ec);

			f(ec, results);
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}