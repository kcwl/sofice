#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

namespace asmpp
{
	class ctx_insert
	{
	public:
		explicit ctx_insert(std::shared_ptr<service> service_ptr)
			: service_ptr_(service_ptr)
		{
		}

	public:
		template<typename T, typename Func>
		void query(T value, Func&& f)
		{
			auto sql = asmpp::detail::template generate<insert_mode, T>::sql(std::move(value), {});

			error_code ec;

			service_ptr_->query(sql,ec);

			f(ec);
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}