#pragma once
#include <vector>
#include <atomic>
#include <shared_mutex>

namespace aquarius
{
	template<class T,class Container = std::vector<T>>
	class circle_buffer
	{
	public:
		using reference = T&;
		using const_reference = const T&;
		using value_type = T;
		using pointer = T*;
		using size_type = std::size_t;
		using const_pointer = const T*;

		inline constexpr static size_type capacity = 6;

	public:
		circle_buffer() = default;

		explicit circle_buffer(const size_type& size)
		{
			container_.clear();
			container_.resize(size);
		}

	public:
		reference operator[](size_type pos)
		{
			return container_[pos];
		}

	public:
		bool push_back(const T& val)
		{
			std::shared_lock<std::shared_mutex> lk(mutex_);

			full() ? total_uses_ = 0 : 0;
			
			container_.at(total_uses_++) = val;

			return true;
		}

		bool empty()
		{
			return container_.empty();
		}

		bool full()
		{
			return total_uses_ == container_.size();
		}

		void resize_util(size_type size)
		{
			return container_.resize(size);
		}

		void resize_for(size_type size)
		{
			return container_.resize(size + container_.size());
		}

		reference front()
		{
			std::shared_lock<std::shared_mutex> lk(mutex_);

			cur_pos_ == container_.size() ? cur_pos_ = 0 : 0;

			return container_.at(cur_pos_);
		}

	private:
		void clear()
		{
			return container_.clear();
		}

	private:
		std::atomic<size_type> cur_pos_;
		std::atomic<size_type> total_uses_;

		std::shared_mutex mutex_;

		Container container_;
	};
}