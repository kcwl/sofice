#pragma once
#include "service_pool.hpp"
#include <functional>
#include "select.hpp"
#include "update.hpp"

class context
{
public:
	explicit context(const std::string& host, const std::string& user, const std::string& passwd, const std::string& db, int port)
		: pool_(host,user,passwd,db,port)
	{

	}

public:
	void run()
	{
		return pool_.run();
	}

public:
	template<typename T,typename Func>
	void select(Func&& f)
	{
		select_context{pool_.get_service()}.excute<T>(std::forward<Func>(f));
	}

	template<typename T>
	void insert(const T& value) 
	{
		update_context{pool_.get_service()}.excute<T, sofice::insert_mode>(value);
	}

	template<typename T>
	void update(const T& value)
	{
		update_context{pool_.get_service()}.excute<T, sofice::update_mode>(value);
	}

	template<typename T>
	void remove() 
	{
		update_context{pool_.get_service()}.excute<T, sofice::delete_mode>();
	}

private:
	service_pool pool_;
};