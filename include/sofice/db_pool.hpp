#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include "circle_buffer.hpp"
#include "detail/singleton.hpp"

namespace sofice
{

	template<class T>
	class db_pool : public detail::singleton<db_pool<T>>
	{
		inline constexpr static std::size_t pool_size = 6 * 2;

	public:
		template<class... Args>
		void init_pool(int size, Args&&... args)
		{
			static_assert(sizeof...(Args) == 5, "connect param is not enough!");

			std::call_once(once_flag_, &db_pool<T>::template init_impl<Args...>, this, size, std::forward<Args>(args)...);
		}

		std::shared_ptr<T> get()
		{
			auto iter = pool_.pop_front();

			std::shared_ptr<T> conn_ptr;

			iter == pool_.end() ? conn_ptr = create() : conn_ptr = *iter;

			//链接超时
			auto now = std::chrono::system_clock::now();
			auto last = conn_ptr->get_last_operate_time();
			auto mins = std::chrono::duration_cast<std::chrono::minutes>(now - last).count();

			if (mins > conn_ptr->get_timeout())
			{
				conn_ptr = create();

				this->release_connect(conn_ptr);

				conn_ptr = *pool_.pop_front();
			}

			conn_ptr->update_last_operate_time();

			return conn_ptr;
		}
		void release_connect(std::shared_ptr<T> ptr)
		{
			pool_.push_back(ptr);
		}

	private:
		auto create()
		{
			auto conn_ptr = std::make_shared<T>();
			auto func = [conn_ptr, this] (auto... args){
				return conn_ptr->connect(args...);
			};

			return std::apply(func, connect_args_) ? conn_ptr : nullptr;
		}

		template<class... Args>
		void init_impl(int size, Args&&... args)
		{
			connect_args_ = std::make_tuple(std::forward<Args>(args)...);

			for (int i = 0; i < size; i++)
			{
				auto conn_ptr = create();

				if (conn_ptr == nullptr)
					continue;

				pool_.push_back(conn_ptr);

				conn_ptr->update_last_operate_time();
			}
		}

	private:
		ring_buffer<std::shared_ptr<T>> pool_;

		std::string db_name_;

		std::once_flag once_flag_;

		std::tuple<const char*, const char*, const char*, const char*, int> connect_args_;
	};
}