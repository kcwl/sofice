#pragma once
#include <mysqlx/xdevapi.h>
#include "mysql_collection.hpp"

namespace aquarius
{
	namespace detail
	{
		namespace mysqld
		{
			class mysql_schema
			{
			public:
				mysql_schema(mysqlx::Session* sess_ptr, const std::string& dbname)
					: schema_ptr_(new mysqlx::Schema(*sess_ptr,dbname)) 
					, collection_ptr_(new mysqld::mysql_collection()){ }

			public:
				bool create_collection(const std::string& table_name)
				{
					try
					{
						schema_ptr_->createCollection(table_name);
						 
						return true;
					}
					catch(...) { }

					return false;
				}

				template<class T>
				auto get_collection()
				{
					return collection_ptr_;
				}

				template<class T>
				auto create_table(const std::string& engine, const std::string& charset)
				{
					return detail::generate_create_table_sql(T{}, engine, charset);
				}

				template<class T>
				auto delete_table()
				{
					return detail::generate_delete_table_sql<T>();
				}

			private:
				std::shared_ptr<mysqlx::Schema> schema_ptr_;
				std::shared_ptr<mysqld::mysql_collection> collection_ptr_;
			};
		}
	}
}