#pragma once
#include <string>

namespace asmpp
{
	class error_code
	{
	public:
		error_code()
			: code_(0)
			, error_() { }

		error_code(const std::string& err,std::size_t code)
			: code_(code)
			, error_(err)
		{ }

		error_code& operator=(const error_code& error)
		{
			if(this != &error)
			{
				this->error_ = error.what();
			}
				
			return *this;
		}
	public:
		std::string what() const
		{
			return !error_.empty()?error_:"Unknown Error!";
		}

		std::size_t error() const
		{
			return code_;
		}

	private:
		std::string error_;

		std::size_t code_;
	};
}