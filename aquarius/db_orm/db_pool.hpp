#pragma once
#include <memory>
#include <mutex>
#include "detail/db_core.hpp"
#include "detail/circle_buffer.hpp"

namespace aquarius
{
	namespace pool
	{
		template<class Connect>
		class db_connect_pool
		{
		public:
			static db_connect_pool& get_instance()
			{
				static db_connect_pool<Connect> instance_;

				return instance_;
			}

			~db_connect_pool() = default;

		public:
			template<class... Args>
			void init(int size, Args&&... args)
			{
				std::call_once(once_flag_, &db_connect_pool<Connect>::template init_impl<Args...>, this, size, std::forward<Args>(args)...);
			}

			std::shared_ptr<db_core<Connect>> get()
			{
				auto conn_ptr = pool_.front();

				//链接超时
				auto now = std::chrono::system_clock::now();
				auto last = conn_ptr->get_last_operate_time();
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now - last).count();
				if(mins > conn_ptr->get_timeout())
					return conn_ptr = create();

				//未就绪
				if(!conn_ptr->is_complete())
					return conn_ptr = create();

				conn_ptr->update_last_operate_time();

				return conn_ptr;
			}
			void release_connect(std::shared_ptr<db_core<Connect>> ptr)
			{
				if(!ptr->is_complete())
					ptr = create();

				pool_.push_back(ptr);
			}

		private:
			db_connect_pool()
				: pool_(6)
			{

			}
			
			db_connect_pool(const db_connect_pool&) = delete;
			db_connect_pool& operator=(const db_connect_pool&) = delete;

		private:
			auto create()
			{
				auto conn_ptr = std::make_shared<db_core<Connect>>();
				auto func = [conn_ptr](auto... args)
				{
					return conn_ptr->connect(args...);
				};

				return std::apply(func, connect_args_) ? conn_ptr : nullptr;
			}

			template<class... Args>
			void init_impl(int size, Args&&... args)
			{
				connect_args_ = std::make_tuple(std::forward<Args>(args)...);

				for(int i = 0; i < size; i++)
				{
					auto conn_ptr = std::make_shared<db_core<Connect>>();
					if(!conn_ptr->connect(std::forward<Args>(args)...))
						continue;

					pool_.push_back(conn_ptr);
				}
			}

		private:
			circle_buffer<std::shared_ptr<db_core<Connect>>> pool_;

			std::once_flag once_flag_;

			std::tuple<const char*, const char*, const char*, const char*, int> connect_args_;
		};

		template<class Pool, class Connect>
		class db_connect_pool_guard
		{
		public:
			db_connect_pool_guard(Pool& pool, std::shared_ptr<db_core<Connect>> conn_ptr)
				: pool_(pool)
				, conn_ptr_(conn_ptr)
			{
			}

			~db_connect_pool_guard()
			{
				pool_.release_connect(conn_ptr_);
			}

		private:
			std::shared_ptr<db_core<Connect>> conn_ptr_;
			Pool& pool_;
		};
	}
}