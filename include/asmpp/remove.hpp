#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

namespace asmpp
{
	class ctx_remove
	{
	public:
		explicit ctx_remove(std::shared_ptr<service> service_ptr)
			: service_ptr_(service_ptr)
		{
		}

	public:
		template<typename T, typename Func>
		void query(const std::string& condition, Func&& f)
		{
			auto sql = asmpp::detail::template generate<remove_mode, T>::sql(condition);

			error_code ec;

			service_ptr_->query(sql, ec);

			f(ec);
		}

		template<typename T, typename Func>
		void query(const std::string& condition="")
		{
			this->template query(condition, [](auto ec){});
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}