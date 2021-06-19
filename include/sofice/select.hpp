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
		void excute(Func&& f)
		{
			auto sql = asmpp::detail::template generate<asmpp::select_mode, T>::sql();

			auto results = service_ptr_->real_query<T>(sql);

			if(results.empty())
				return;

			f(results);
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}