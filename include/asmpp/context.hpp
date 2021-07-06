#pragma once
#include "service_pool.hpp"
#include <functional>
#include "select.hpp"
#include "update.hpp"
#include "insert.hpp"
#include "remove.hpp"

namespace asmpp
{
	class context
	{
	public:
		explicit context(const std::string& host, const std::string& user, const std::string& passwd, const std::string& db, int port)
			: pool_(host, user, passwd, db, port)
		{

		}

	public:
		void run()
		{
			return pool_.run();
		}

	public:
		template<typename T, typename Func>
		void select(const std::string& condition, Func&& f)
		{
			ctx_select{pool_.get_service()}.query_if<T,Func>(condition, std::forward<Func>(f));
		}

		template<typename T, typename Func>
		void select(const std::string& condition)
		{
			ctx_select{pool_.get_service()}.query_if<T, Func>(condition, [](auto ec, auto result){});
		}

		template<typename T,typename Func>
		void insert(const T& value,Func&& f)
		{
			ctx_insert{pool_.get_service()}.query<T>(value,std::forward<Func>(f));
		}

		template<typename T>
		void insert(const T& value)
		{
			ctx_insert{pool_.get_service()}.query<T>(value, [](auto ec){});
		}

		template<typename T,typename Func>
		void insert(const std::vector<T>& values,Func&& f)
		{
			for(auto& iter : values)
				insert(iter, std::forward<Func>(f));
		}

		template<typename T,typename Func>
		void update(const T& value,const std::string& condition, Func&& f)
		{
			ctx_update{pool_.get_service()}.query<T>(value,condition,std::forward<Func>(f));
		}

		template<typename T, typename Func>
		void update(const T& value, const std::string& condition="")
		{
			ctx_update{pool_.get_service()}.query<T>(value, condition, [](auto ec){});
		}

		template<typename T>
		void remove()
		{
			ctx_remove{pool_.get_service()}.query<T>();
		}

		//template<typename T>
		//void create()
		//{
		//	ctx_update{pool_.get_service()}.excute<T, asmpp::create_mode>();
		//}

		template<typename T>
		T query_one(std::string sql)
		{
			auto result = ctx_select{pool_.get_service()}.query<T>(sql);

			if(result.empty())
				return T{};

			return result.at(0);
		}

	private:
		service_pool pool_;
	};
}