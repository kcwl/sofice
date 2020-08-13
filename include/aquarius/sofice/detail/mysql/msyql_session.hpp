#pragma once
#include <locale>
#include <codecvt>
#include <mysqlx/xdevapi.h>
#include "mysql_schema.hpp"

namespace aquarius
{
	namespace detail
	{
		namespace mysqld
		{
			class mysql_session
			{
			public:
				template<class... Args>
				mysql_session(Args&&... args)
					: session_ptr_(new mysqlx::Session(std::forward<Args>(args)...)) { }

			public:
				auto create_database(const std::string& dbname)
				{
					try
					{
						session_ptr_->createSchema(dbname);

						return schema_ptr_ = std::make_shared<mysqld::mysql_schema>(session_ptr_.get(),dbname);
					}
					catch(...) { }

					sql("use " + dbname + ";");

					return schema_ptr_ = std::shared_ptr<mysqld::mysql_schema>(new mysqld::mysql_schema(session_ptr_.get(), dbname));
				}

				auto get_schema()
				{
					return schema_ptr_;
				}

				void close()
				{
					try
					{
						session_ptr_->close();
					}
					catch(...) { }
				}

				auto sql(const std::string& str_sql) ->mysqlx::SqlResult
				{
					//sql gbk to utf8
					try
					{
						return session_ptr_->sql(to_utf8(str_sql)).execute();
					}
					catch(std::exception& e) 
					{ 
						std::cout << e.what() << std::endl;
					}

					return mysqlx::SqlResult{};
				}

				void start_transaction()
				{
					try
					{
						session_ptr_->startTransaction();
					}
					catch(...) { }
				}

				void roll_back()
				{
					try
					{
						session_ptr_->rollback();
					}
					catch(...) { }
				}

				void commit()
				{
					try
					{
						session_ptr_->commit();
					}
					catch(...) {}
				}

				auto add(const std::string& str)
				{
					auto schema = session_ptr_->getSchema("test1");
					auto db = schema.getCollection("person");
					db.add(str).execute();
				}

			private:
				std::string to_utf8(const std::string& str)
				{
					std::vector<wchar_t> buff(str.size());
#ifdef _MSC_VER
					std::locale loc("zh-CN");
#else
					std::locale loc("zh-CN.GB18030");
#endif
					wchar_t* pwsz = nullptr;
					const char* sz = nullptr;

					std::mbstate_t state{};

					std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc).in(state, str.data(), str.data() + str.size(), sz, buff.data(), buff.data() + buff.size(), pwsz);

					return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes(std::wstring(buff.data(), buff.size()));
				}

			private:
				std::shared_ptr<mysqlx::Session> session_ptr_;

				std::shared_ptr<mysqld::mysql_schema> schema_ptr_;
			};
		}
	}
}