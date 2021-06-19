#pragma once
#include "sofice/db_pool.hpp"
#include "sofice/db_operator.hpp"
#include "sofice/mysql.hpp"
#include "sofice/detail/algorithm.hpp"

#include "sofice/context.hpp"

namespace sofice
{
	template<class T>
	using dbpool = db_pool<T>;

	using mysql_pool = db_pool<detail::mysql>;
}