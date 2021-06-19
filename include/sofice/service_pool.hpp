#pragma once
#include "service.hpp"
#include <memory>
#include <vector>
#include <thread>
#include <algorithm>

class service_pool
{
	using service_ptr = std::shared_ptr<service>;

	inline constexpr static std::size_t pool_size = 2 * 3;

public:
	explicit service_pool(const std::string& host, const std::string& user, const std::string& passwd, const std::string& db, int port, std::size_t size = pool_size)
		: service_pos_(0)
	{
		for(std::size_t i = 0; i < size; i++)
		{
			service_pool_.push_back(std::make_shared<service>(host, user, passwd, db, port));
		}
	}

	void run()
	{
		//mysql_init(nullptr);

		std::vector<std::shared_ptr<std::thread>> thread_pool_;
		std::for_each(service_pool_.begin(), service_pool_.end(), [&thread_pool_](auto iter)
					  {
						  thread_pool_.push_back(std::make_shared<std::thread>([it = std::move(iter)]
												 {
													 it->run();
												 }));
					  });

		std::for_each(thread_pool_.begin(), thread_pool_.end(), [](auto iter)
					  {
						  iter->join();
					  });
	}

	void stop()
	{
		std::for_each(service_pool_.begin(), service_pool_.end(), [](auto iter)
					  {
						  iter->shutdown();
					  });
	}

	service_ptr get_service()
	{
		if(service_pos_ == service_pool_.size())
			service_pos_ = 0;

		return service_pool_.at(service_pos_++);
	}

private:
	std::vector<service_ptr> service_pool_;

	std::size_t service_pos_;
};