#pragma once
#include "service_pool.hpp"
#include <functional>
#include "select.hpp"
#include "update.hpp"

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
		void select(Func&& f,std::string condition = "")
		{
			select_context{pool_.get_service()}.excute<T>(std::forward<Func>(f),condition);
		}

		template<typename T>
		void insert(const T& value)
		{
			update_context{pool_.get_service()}.excute<T, asmpp::insert_mode>(value);
		}

		template<typename T>
		void insert(const std::vector<T>& values)
		{
			for(auto& iter : values)
				insert(iter);
		}

		template<typename T>
		void update(const T& value)
		{
			update_context{pool_.get_service()}.excute<T, asmpp::update_mode>(value,"");
		}

		template<typename T>
		void remove()
		{
			update_context{pool_.get_service()}.excute<T, asmpp::delete_mode>();
		}

		template<typename T>
		void create()
		{
			update_context{pool_.get_service()}.excute<T, asmpp::create_mode>();
		}

		template<typename T>
		T query(std::string sql)
		{
			auto result = select_context{pool_.get_service()}.query<T>(sql);

			if(result.empty())
				return T{};

			return result.at(0);
		}

	private:
		service_pool pool_;
	};
}