﻿#pragma once
#include <string>
#include <vector>
#include <mysql.h>
#include "detail/generate_sql.hpp"

namespace sofice
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
			bool connect(Args&&... args)
			{
				if (conn_ptr_ != nullptr)
					mysql_close(conn_ptr_);

				disconnect();

				connect_args_ = std::make_tuple(std::forward<Args>(args)...);

				conn_ptr_ = mysql_init(nullptr);

				if (conn_ptr_ == nullptr)
				{
					set_last_error("mysql init error ! please check that no memory problem !");
					return false;
				}

				set_default_option();

				auto tp = std::tuple_cat(std::make_tuple(conn_ptr_), connect_args_, std::make_tuple(nullptr, 0));

				if (std::apply(mysql_real_connect, tp) == nullptr)
				{
					set_last_error(mysql_error(conn_ptr_));
					mysql_close(conn_ptr_);
					return false;
				}

				return true;
			}

			bool disconnect()
			{
				if (conn_ptr_)
					mysql_close(conn_ptr_);

				return true;
			}

			std::string error()
			{
				return error_;
			}

			bool excute_sql(const std::string& sql)
			{
				mysql_query(conn_ptr_, sql.c_str());
				return true;
			}

			bool excute_no_sql(const std::string& sql)
			{
				return mysql_query(conn_ptr_,sql.c_str()) == 0;
			}

			template<class T>
			auto create_table(const std::string& engine = "InnoDB", const std::string& charset = "utf8")
			{
				auto sql = detail::create_table_sql(T{}, engine, charset);

				return excute_no_sql(sql);
			}

			template<class T>
			auto delete_table()
			{
				return excute_no_sql("");
			}

			template<class T>
			auto insert(T&& t)
			{
				return excute_no_sql(detail::generate_sql<insert_mode>(t));
			}

			template<class T, class = std::void_t<decltype(std::declval<T>().begin()),decltype(std::declval<T>().end())>>
			auto insert(T&& vec)
			{
				start_transaction();

				std::for_each(vec.begin(), vec.end(), [this](auto iter)
					{
						if (insert(iter) != 0)
							return true;

						mysql_rollback(conn_ptr_);
						set_last_error("insert failed!");
						return false;
					});

				mysql_commit(conn_ptr_);

				return true;
			}

			bool start_transaction()
			{
				if (!mysql_query(conn_ptr_, "start transaction"))
				{
					set_last_error(mysql_error(conn_ptr_));
					return false;
				}

				if (!mysql_autocommit(conn_ptr_, 1))
				{
					set_last_error(mysql_error(conn_ptr_));
					return false;
				}

				return true;
			}

			template<class T>
			auto update(T&& t, const std::string& condition)
			{
				auto sql = detail::generate_sql<update_mode>(t);

				return excute_no_sql(sql);
			}

			template<class T>
			auto remove(const std::string& condition = "")
			{
				auto sql = detail::generate_sql(T{}, delete_mode{});

				return excute_no_sql(sql);
			}

			template<class T>
			auto select(const std::string& condition = "")
			{
				std::vector<T> results{};

				auto sql = detail::generate_sql<select_mode>(T{});

				excute_sql(sql + condition);

				auto res = mysql_store_result(conn_ptr_);

				while (auto column = mysql_fetch_row(res))
				{
					results.push_back(detail::construct_data<T>(column));
				}

				return results;
			}

			int get_timeout()
			{
				return time_out_;
			}

			auto get_last_operate_time()
			{
				return last_operate_time_;
			}

			void update_last_operate_time()
			{
				last_operate_time_ = std::chrono::system_clock::now();
			}

		private:
			void set_last_error(std::string&& error)
			{
				error_ = std::move(error);
			}

			void set_default_option()
			{
				mysql_options(conn_ptr_, MYSQL_OPT_RECONNECT, &auto_reconnect_);

				mysql_options(conn_ptr_, MYSQL_SET_CHARSET_NAME, "utf8");

				mysql_options(conn_ptr_, MYSQL_OPT_CONNECT_TIMEOUT, &time_out_);
			}


		private:
			MYSQL* conn_ptr_ = nullptr;

			std::string error_;

			int time_out_ = 3000;

			std::chrono::system_clock::time_point last_operate_time_{};

			bool auto_reconnect_ = true;

			std::tuple<const char*, const char*, const char*, const char*, int> connect_args_;
		};
	}
}