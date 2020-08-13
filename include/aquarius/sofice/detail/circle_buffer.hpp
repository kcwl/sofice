#pragma once
#include <vector>
#include <atomic>
#include <shared_mutex>

namespace aquarius
{
	namespace detail
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
				buffer_.clear();
				buffer_.resize(size);
			}

		public:
			reference operator[](size_type pos)
			{
				return buffer_[pos];
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

			bool empty()
			{
				return buffer_.empty();
			}

			bool full()
			{
				return total_uses_ == buffer_.size();
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

			iterator pop_front()
			{
				std::shared_lock<std::shared_mutex> lk(mutex_);

				if(buffer_.empty())
					return buffer_.end();

				total_uses_--;

				return buffer_.erase(buffer_.begin());
			}

		private:
			void clear()
			{
				return buffer_.clear();
			}

		private:
			std::atomic<size_type> total_uses_;

			std::shared_mutex mutex_;

			Container buffer_;
		};
	}

	template<class T>
	using circle_buffer = detail::circle_buffer<T, std::vector<T>>;
}