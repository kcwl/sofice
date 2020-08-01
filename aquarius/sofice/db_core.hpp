#pragma once
#include <chrono>
#include <memory>
#include <string>

namespace aquarius
{
	namespace sofice
	{
		template<class T>
		class db_core
		{
		public:
			db_core()
				: core_ptr_(new T()) { }

			~db_core() = default;

		public:
			template<class... Args>
			bool connect(const std::string& dbname, Args&&... args)
			{
				return core_ptr_->connect(dbname, std::forward<Args>(args)...);
			}

			bool disconnect()
			{
				return core_ptr_->disconnect();
			}

			int excute(const std::string& sql)
			{
				return core_ptr_->excute(sql.c_str());
			}

			template<class T>
			auto create_collection()
			{
				return core_ptr_->create_collection<T>();
			}

			template<class T>
			auto create_table()
			{
				return core_ptr_->create_table<T>();
			}

			template<class T>
			auto delete_table()
			{
				return core_ptr_->delete_table<T>();
			}

			template<class T>
			auto insert(const T& t)
			{
				return core_ptr_->insert(t);
			}

			template<class T>
			auto update(const T& t, const std::string& condition = "")
			{
				return core_ptr_->update(t, condition);
			}

			template<class T>
			auto remove(const std::string& condition = "")
			{
				return core_ptr_->remove<T>(condition);
			}

			template<class T>
			auto select(const std::string& condition = "")
			{
				return core_ptr_->select<T>(condition);
			}

			auto get_last_operate_time()
			{
				return last_;
			}

			void update_last_operate_time()
			{
				last_ = std::chrono::system_clock::now();
			}

			auto create_database(const std::string& dbname)
			{
				return core_ptr_->create_database(dbname);
			}

			int get_timeout()
			{
				return core_ptr_->get_timeout();
			}

			auto add(const std::string& str)
			{
				core_ptr_->add(str);
			}
		private:
			std::unique_ptr<T> core_ptr_;

			std::chrono::time_point<std::chrono::system_clock> last_;
		};
	}
}
