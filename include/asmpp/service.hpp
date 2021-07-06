#pragma once
#include <mysql.h>
#include <string>
#include "detail/algorithm.hpp"
#include "error_code.hpp"

namespace asmpp
{
	class service
	{
	public:
		explicit service(const std::string& host, const std::string& user, const std::string& passwd, const std::string& db, int port)
			: mysql_ptr_(nullptr)
			, host_(host)
			, user_(user)
			, passwd_(passwd)
			, db_(db)
			, port_(port)
			, auto_reconnect_(true)
			, charset_("utf8")
		{
		}

		virtual ~service()
		{

		}

	public:
		inline bool run()
		{
			disconnect();

			mysql_ptr_ = mysql_init(nullptr);

			if(mysql_ptr_ == nullptr)
			{
				return false;
			}

			set_default_option();

			auto tp = std::make_tuple(mysql_ptr_, host_.data(), user_.data(), passwd_.data(), db_.data(), port_, nullptr, 0);

			if(std::apply(mysql_real_connect, tp) == nullptr)
			{
				return false;
			}

			return true;
		}

		inline void shutdown()
		{
			return disconnect();
		}

		void set_charset(const std::string& charset = "utf8")
		{
			charset_ = charset;
		}

		std::size_t query(const std::string& sql,error_code& error)
		{
			auto res = mysql_query(mysql_ptr_, sql.c_str());
			error = error_code(mysql_error(mysql_ptr_), mysql_errno(mysql_ptr_));
			return res;
		}

		template<typename T>
		std::vector<T> real_query(const std::string& sql,error_code& ec)
		{
			std::vector<T> results;

			if(mysql_ptr_ == nullptr)
				return results;

			auto res_err = query(sql);

			if(res_err != 0)
			{
				auto str = mysql_error(mysql_ptr_);

				ec = error_code(mysql_error(mysql_ptr_), mysql_errno(mysql_ptr_));

				return results;
			}

			auto res = mysql_store_result(mysql_ptr_);

			if(res == nullptr)
				return results;

			while(auto column = mysql_fetch_row(res))
			{
				results.push_back(asmpp::detail::template to_struct<T>(column));
			}

			return results;
		}

	private:
		void disconnect()
		{
			if(mysql_ptr_ == nullptr)
				return;

			mysql_close(mysql_ptr_);
		}

		void set_default_option()
		{
			mysql_options(mysql_ptr_, MYSQL_OPT_RECONNECT, &auto_reconnect_);

			mysql_options(mysql_ptr_, MYSQL_SET_CHARSET_NAME, charset_.data());
		}

	private:
		MYSQL* mysql_ptr_;
		std::string host_;
		std::string user_;
		std::string passwd_;
		std::string db_;
		unsigned int port_;

		bool auto_reconnect_;
		std::string charset_;
	};
}