#pragma once
#include <memory>
#include <mutex>
#include <thread>
#include "db_core.hpp"
#include "detail/circle_buffer.hpp"

namespace aquarius
{
	namespace sofice
	{
		template<class T>
		class db_pool
		{
			inline constexpr static std::size_t pool_size = 6 * 2;
		public:
			static db_pool& get_instance()
			{
				static db_pool<T> instance_;

				return instance_;
			}

			~db_pool() = default;

		public:
			template<class... Args>
			void init_pool(int size, const std::string& dbname,  Args&&... args)
			{
				static_assert(sizeof...(Args) == 4, "connect param is not enough!");

				std::call_once(once_flag_, &db_pool<T>::template init_impl<Args...>, this, size, dbname, std::forward<Args>(args)...);
			}

			std::shared_ptr<db_core<T>> get()
			{
				auto iter = pool_.pop_front();

				std::shared_ptr<db_core<T>> conn_ptr;

				iter == pool_.end() ? conn_ptr = create() : conn_ptr = *iter;

				//链接超时
				auto now = std::chrono::system_clock::now();
				auto last = conn_ptr->get_last_operate_time();
				auto mins = std::chrono::duration_cast<std::chrono::minutes>(now - last).count();

				if(mins > conn_ptr->get_timeout())
				{
					conn_ptr = create();

					this->release_connect(conn_ptr);

					conn_ptr = *pool_.pop_front();
				}

				conn_ptr->update_last_operate_time();

				return conn_ptr;
			}
			void release_connect(std::shared_ptr<db_core<T>> ptr)
			{
				pool_.push_back(ptr);
			}

		private:
			db_pool()
				: pool_(pool_size)
			{

			}
			
			db_pool(const db_pool&) = delete;
			db_pool& operator=(const db_pool&) = delete;

		private:
			auto create()
			{
				auto conn_ptr = std::make_shared<db_core<T>>();
				auto func = [conn_ptr,this](auto... args)
				{
					return conn_ptr->connect(db_name_, args...);
				};

				return std::apply(func, connect_args_) ? conn_ptr : nullptr;
			}

			template<class... Args>
			void init_impl(int size, const std::string& dbname, Args&&... args)
			{
				db_name_ = dbname;

				connect_args_ = std::make_tuple(std::forward<Args>(args)...);

				for(int i = 0; i < size; i++)
				{
					auto conn_ptr = create();

					if(conn_ptr == nullptr)
						continue;

					pool_.push_back(conn_ptr);

					conn_ptr->update_last_operate_time();
				}
			}

		private:
			circle_buffer<std::shared_ptr<db_core<T>>> pool_;

			std::string db_name_;

			std::once_flag once_flag_;

			std::tuple<const char*, int, const char*, const char*> connect_args_;
		};

		template<class Pool, class T>
		class db_connect_pool_guard
		{
		public:
			db_connect_pool_guard(Pool& sofice, std::shared_ptr<db_core<T>> conn_ptr)
				: pool_(sofice)
				, conn_ptr_(conn_ptr)
			{
			}

			~db_connect_pool_guard()
			{
				pool_.release_connect(conn_ptr_);
			}

		private:
			std::shared_ptr<db_core<T>> conn_ptr_;
			Pool& pool_;
		};
	}
}