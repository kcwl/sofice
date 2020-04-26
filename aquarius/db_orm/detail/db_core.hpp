#pragma once
#include <chrono>
#include <memory>
#include <string>

namespace aquarius
{
	namespace pool
	{
		template<class Core>
		class db_core
		{
		public:
			db_core()
				: core_ptr_(new Core()) { }
			~db_core() = default;

		public:
			template<class... Args>
			bool connect(Args&&... args)
			{
				return core_ptr_->connect(std::forward<Args>(args)...);
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
			int insert(const T& t)
			{
				return core_ptr_->insert(t);
			}

			template<class T>
			int update(const T& t, const std::string& condition = "")
			{
				return core_ptr_->update(t, condition);
			}

			template<class T>
			int delete_from_table(const std::string& condition = "")
			{
				return core_ptr_->delete_from_table<T>(condition);
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

			void rollback()
			{
				return core_ptr_->rollback();
			}

			bool commit()
			{
				return core_ptr_->commit();
			}

			void update_last_operate_time()
			{
				last_ = std::chrono::system_clock::now();
			}

			bool is_complete()
			{
				return core_ptr_->is_complete();
			}

			void create_database(const std::string& dbname)
			{
				return core_ptr_->create_database(dbname);
			}

			int get_timeout()
			{
				return core_ptr_->get_timeout();
			}
		private:
			std::unique_ptr<Core> core_ptr_;

			std::chrono::time_point<std::chrono::system_clock> last_;
		};
	}
}
