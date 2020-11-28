#pragma once
#include <vector>
#include <atomic>
#include <shared_mutex>

namespace sofice
{

	template<class T, class Container>
	class circle_buffer
	{
	public:
		using reference = T&;
		using const_reference = const T&;
		using value_type = T;
		using pointer = T*;
		using size_type = std::size_t;
		using const_pointer = const T*;
		using iterator = typename Container::iterator;

		inline constexpr static size_type capacity = 6;

	public:
		circle_buffer() = default;

		explicit circle_buffer(const size_type& size)
		{
			clear();
			buffer_.resize(size);
		}

	public:
		reference operator[](size_type pos)
		{
			return buffer_.at(pos);
		}

	public:
		iterator begin()
		{
			return buffer_.begin();
		}

		iterator end()
		{
			return buffer_.end();
		}

		bool push_back(const T& val)
		{
			std::shared_lock<std::shared_mutex> lk(mutex_);

			full() ? total_uses_ = 0 : 0;

			buffer_.at(total_uses_++) = val;

			return true;
		}

		iterator pop_front()
		{
			std::shared_lock<std::shared_mutex> lk(mutex_);

			if (empty())
				return buffer_.end();

			total_uses_--;

			return buffer_.erase(buffer_.begin());
		}

		void resize_util(size_type size)
		{
			std::shared_lock<std::shared_mutex> lk(mutex_);

			return buffer_.resize(size);
		}

		void resize_for(size_type size)
		{
			std::shared_lock<std::shared_mutex> lk(mutex_);

			return buffer_.resize(size + buffer_.size());
		}

	private:
		void clear()
		{
			return buffer_.clear();
		}

		bool empty()
		{
			return buffer_.empty();
		}

		bool full()
		{
			return total_uses_ == buffer_.size();
		}

	private:
		std::atomic<size_type> total_uses_;

		std::shared_mutex mutex_;

		Container buffer_;
	};

	template<class T>
	using ring_buffer = circle_buffer<T, std::vector<T>>;
}