#pragma once
#include "db_orm/db_pool.hpp"
#include "db_orm/db_operator.hpp"
#include "db_orm/detail/utility.hpp"
#include "db_orm/detail/mysql.hpp"

namespace aquarius
{
	template<class Connect>
	using db_pool = pool::db_connect_pool<Connect>;

	template<class Pool,class Connect>
	using db_pool_guard = pool::db_connect_pool_guard<Pool, Connect>;

	using mysql_pool = pool::db_connect_pool<mysql>;
}