#pragma once
#include <string>
#include "utility.hpp"
#include <vector>

namespace aquarius
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
			if(conn_ptr_ != nullptr)
				mysql_close(conn_ptr_);

			connect_args_ = std::make_tuple(std::forward<Args>(args)...);

			conn_ptr_ = mysql_init(nullptr);

			if(conn_ptr_ == nullptr)
			{
				set_last_error(mysql_error(conn_ptr_));
				return false;
			}

			set_default_option();

			auto tp = std::tuple_cat(std::make_tuple(conn_ptr_), connect_args_, std::make_tuple(nullptr, 0));

			if(std::apply(mysql_real_connect, tp) == nullptr)
			{
				set_last_error(mysql_error(conn_ptr_));
				mysql_close(conn_ptr_);
				return false;
			}

			return true;
		}

		bool disconnect()
		{
			if(conn_ptr_)
				mysql_close(conn_ptr_);

			return true;
		}

		std::string error()
		{
			return error_;
		}

		int excute(const std::string& sql)
		{
			return mysql_query(conn_ptr_, sql.c_str());
		}

		bool start()
		{
			if(!mysql_query(conn_ptr_, "start transaction"))
			{
				set_last_error(mysql_error(conn_ptr_));
				return false;
			}

			if(!mysql_autocommit(conn_ptr_, 1))
			{
				set_last_error(mysql_error(conn_ptr_));
				return false;
			}

			return true;
		}

		auto commit()
		{
			return mysql_commit(conn_ptr_);
		}

		auto rollback()
		{
			return mysql_rollback(conn_ptr_);
		}

		template<class T>
		auto create_table(const std::string& engine = "InnoDB",const std::string& charset="utf8")
		{
			auto sql = /*generate::create::generate_sql<T>()*/generate::generate_create_table_sql<T>(T{}, engine, charset);

			return excute(sql);
		}

		template<class T>
		auto delete_table()
		{
			auto sql = generate::generate_delete_table_sql<T>();

			return excute(sql);
		}

		template<class T>
		auto insert(T& t)
		{
			auto sql = /*generate::insert::generate_sql<T>(t)*/generate::generate_sql<insert_mode>(t);

			return excute(sql);
		}

		template<class T>
		auto insert(std::vector<T>& vec)
		{
			start();
			for(auto iter : vec)
			{
				if(insert(iter) == 0)
				{
					rollback();
					set_last_error(mysql_error(conn_ptr_));
					return false;
				}
			}

			commit();

			return true;
		}

		template<class T>
		auto update(T& t, const std::string& condition)
		{
			auto sql = /*generate::update::generate_sql<T>(t, condition, std::forward<Args>(args)...)*/generate::generate_sql<update_mode>(t,condition);

			return excute(sql);
		}

		template<class T>
		auto delete_from_table(const std::string& condition = "")
		{
			auto sql = /*generate::del::generate_sql<T>(condition)*/generate::generate_sql<delete_mode>(T{},condition);

			return excute(sql);
		}

		template<class T>
		auto select(const std::string& condition = "")
		{
			std::vector<T> results{};

			auto sql = generate::generate_sql<select_mode>(T{}, condition);

			excute(sql);

			auto res = mysql_store_result(conn_ptr_);

			while(auto column = mysql_fetch_row(res))
			{
				results.push_back(algorithm::construct_data<T>(column));
			}
		}

		bool is_complete()
		{
			return conn_ptr_ != nullptr;
		}

		void create_database(const std::string& dbname, const std::string& charset = "utf8", const std::string& collate = "utf8_general_ci")
		{
			std::string sql = generate::generate_create_database_sql(dbname, charset, collate);

			excute(sql);
		}

		int get_timeout()
		{
			return time_out_;
		}

	private:
		void set_last_error(std::string&& error)
		{
			error_ = std::move(error);
		}

		void set_default_option()
		{
			char value = 1;
			mysql_options(conn_ptr_, MYSQL_OPT_RECONNECT, &value);

			mysql_options(conn_ptr_, MYSQL_SET_CHARSET_NAME, "utf8");

			mysql_options(conn_ptr_, MYSQL_OPT_CONNECT_TIMEOUT, &time_out_);
		}

	private:
		MYSQL* conn_ptr_ = nullptr;

		std::string error_;

		int time_out_ = 3000;

		std::tuple<const char*, const char*, const char*, const char*, int> connect_args_;
	};
}
