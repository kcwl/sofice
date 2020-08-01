#pragma once
#include <string>
#include <vector>
#include <aquarius/reflection.hpp>
#include "mysql/msyql_session.hpp"

namespace aquarius
{
	namespace detail
	{
		class mysql
		{
		public:
			mysql() = default;
			~mysql()
			{
				disconnect();
			}

		public:
			template<class... Args>
			bool connect(const std::string& dbname, Args&&... args)
			{
				//if(conn_ptr_ != nullptr)
				//	mysql_close(conn_ptr_);
				disconnect();

				//connect_args_ = std::make_tuple(std::forward<Args>(args)...);

				//conn_ptr_ = mysql_init(nullptr);

				session_ptr_ = std::make_unique<mysqld::mysql_session>(std::forward<Args>(args)...);

				if(session_ptr_ == nullptr)
				{
					set_last_error("session is null");
					return false;
				}

				//set_default_option();
				create_database(dbname);

				//auto tp = std::tuple_cat(std::make_tuple(conn_ptr_), connect_args_, std::make_tuple(nullptr, 0));

				//if(std::apply(mysql_real_connect, tp) == nullptr)
				//{
				//	set_last_error(mysql_error(conn_ptr_));
				//	mysql_close(conn_ptr_);
				//	return false;
				//}
				

				return true;
			}

			bool disconnect()
			{
				//if(conn_ptr_)
				//	mysql_close(conn_ptr_);
				if(session_ptr_)
					session_ptr_->close();

				return true;
			}

			std::string error()
			{
				return error_;
			}

			auto excute(const std::string& sql)
			{
				return session_ptr_->sql(sql);
			}

			bool excute_no_sql(const std::string& sql)
			{
				try
				{
					excute(sql);
					return true;
				}
				catch(std::exception& e) 
				{
					set_last_error(e.what());
				}
				
				return false;
			}

			template<class T>
			auto create_collection()
			{
				return schema_ptr_->create_collection(std::string(reflect::template tuple_name<T>()));
			}

			template<class T>
			auto create_table(const std::string& engine = "InnoDB", const std::string& charset = "utf8")
			{
				auto sql = schema_ptr_->create_table<T>(engine, charset);

				return excute_no_sql(sql);
			}

			template<class T>
			auto delete_table()
			{
				auto sql = schema_ptr_->delete_table<T>();

				return excute_no_sql(sql);
			}

			template<class T>
			auto insert(T&& t)
			{
				auto collect_ptr = schema_ptr_->get_collection<T>();

				auto sql = collect_ptr->insert(std::forward<T>(t));

				return excute_no_sql(sql);
			}

			template<class T>
			auto insert(const std::vector<T>& vec)
			{
				//start_transaction();
				session_ptr_->start_transaction();

				std::for_each(vec.begin(), vec.end(), [this](auto iter)
					{
						if(insert(iter) != 0)
							return true;

						//mysql_rollback(conn_ptr_);
						session_ptr_->roll_back();
						set_last_error("insert failed!");
						return false;
					});

				//mysql_commit(conn_ptr_);
				session_ptr_->commit();

				return true;
			}

			//bool start_transaction()
			//{
			//	if(!mysql_query(conn_ptr_, "start transaction"))
			//	{
			//		set_last_error(mysql_error(conn_ptr_));
			//		return false;
			//	}

			//	if(!mysql_autocommit(conn_ptr_, 1))
			//	{
			//		set_last_error(mysql_error(conn_ptr_));
			//		return false;
			//	}

			//	return true;
			//}

			template<class T>
			auto update(T&& t, const std::string& condition)
			{
				auto collect_ptr = schema_ptr_->get_collection<T>();
				auto sql = collect_ptr->update(std::forward<T>(t))/*detail::generate_sql(t, update_mode{})*/;

				return excute_no_sql(sql);
			}

			template<class T>
			auto remove(const std::string& condition = "")
			{
				auto collect_ptr = schema_ptr_->get_collection<T>();

				auto sql = collect_ptr->remove<T>(condition)/*detail::generate_sql(T{}, delete_mode{})*/;

				return excute_no_sql(sql);
			}

			template<class T>
			auto select(const std::string& condition = "")
			{
				std::vector<T> results{};

				auto collect_ptr = schema_ptr_->get_collection<T>();

				auto sql = collect_ptr->select<T>(condition)/*detail::generate_sql(T{}, select_mode{})*/;

				excute(sql);

				//auto res = mysql_store_result(conn_ptr_);

				//while(auto column = mysql_fetch_row(res))
				//{
				//	results.push_back(detail::construct_data<T>(column));
				//}

				return results;
			}

			auto create_database(const std::string& dbname, const std::string& charset = "utf8", const std::string& collate = "utf8_general_ci")
			{
				//std::string sql = detail::generate_create_database_sql(dbname, charset, collate);

				//return excute(sql);
				schema_ptr_ = session_ptr_->create_database(dbname);

				return ;
			}

			int get_timeout()
			{
				return time_out_;
			}

			auto add(const std::string& str)
			{
				session_ptr_->add(str);
			}

		private:
			void set_last_error(std::string&& error)
			{
				error_ = std::move(error);
			}

			void set_default_option()
			{
				char value = 1;
				//mysql_options(conn_ptr_, MYSQL_OPT_RECONNECT, &value);

				//mysql_options(conn_ptr_, MYSQL_SET_CHARSET_NAME, "utf8");

				//mysql_options(conn_ptr_, MYSQL_OPT_CONNECT_TIMEOUT, &time_out_);
			}


		private:
			//MYSQL* conn_ptr_ = nullptr;
			std::unique_ptr<mysqld::mysql_session> session_ptr_;

			std::string error_;

			int time_out_ = 3000;

			std::shared_ptr<mysqld::mysql_schema> schema_ptr_;

			std::tuple<const char*, const char*, const char*, const char*, int> connect_args_;
		};
	}
}
