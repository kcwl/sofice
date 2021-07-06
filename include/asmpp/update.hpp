#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

namespace asmpp
{
	class ctx_update
	{
	public:
		explicit ctx_update(std::shared_ptr<service> service_ptr)
			: service_ptr_(service_ptr)
		{
		}

	public:
		template<typename T, typename Func>
		void query(T value,const std::string& condition, Func&& f)
		{
			auto sql = asmpp::detail::template generate<update_mode, T>::sql(std::move(value), condition);

			error_code ec;

			service_ptr_->query(sql,ec);

			f(ec);
		}

		template<typename T, typename Func>
		void query(T value,const std::string& condition="")
		{
			this->template query(value, condition, [](auto ec){});
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}