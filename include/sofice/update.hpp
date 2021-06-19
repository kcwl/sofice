#pragma once
#include "service.hpp"
#include "detail/generate_sql.hpp"

class update_context
{
public:
	explicit update_context(std::shared_ptr<service> service_ptr)
		: service_ptr_(service_ptr)
	{
	}

public:
	template<typename T,typename Tag>
	std::size_t excute(const T& value )
	{
		auto sql = sofice::detail::template generate<Tag, T>::sql(value);

		return service_ptr_->query(sql);
	}

	template<typename T,typename Tag>
	std::size_t excute()
	{
		auto sql = sofice::detail::template generate<Tag, T>::sql();

		return service_ptr_->query<T>(sql);
	}

private:
	std::shared_ptr<service> service_ptr_;
};