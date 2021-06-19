#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

namespace asmpp
{
	class update_context
	{
	public:
		explicit update_context(std::shared_ptr<service> service_ptr)
			: service_ptr_(service_ptr)
		{
		}

	public:
		template<typename T, typename Tag>
		std::size_t excute(T value)
		{
			auto sql = asmpp::detail::template generate<Tag, T>::sql(std::move(value));

			return service_ptr_->query(sql);
		}

		template<typename T, typename Tag>
		std::size_t excute()
		{
			auto sql = asmpp::detail::template generate<Tag, T>::sql();

			return service_ptr_->query(sql);
		}

	private:
		std::shared_ptr<service> service_ptr_;
	};
}