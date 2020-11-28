#pragma once
#include "sofice/db_pool.hpp"
#include "sofice/db_operator.hpp"
#include "sofice/detail/utility.hpp"
#include "sofice/detail/mysql.hpp"
#include "sofice/detail/algorithm.hpp"

namespace sofice
{
	template<class T>
	using dbpool = db_pool<T>;

	template<class Pool,class T>
	using db_pool_guard = sofice::db_connect_pool_guard<Pool, T>;

	using mysql_pool = db_pool<detail::mysql>;
}