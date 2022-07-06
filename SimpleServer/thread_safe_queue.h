#pragma once

#include "net_common.h"

namespace async_component
{
	template<typename T>
	class ThreadSafeQueue
	{
	public:
		ThreadSafeQueue() = default;
		
		ThreadSafeQueue(const ThreadSafeQueue<T>&) = delete;
		
		~ThreadSafeQueue()
		{
			queue_.clear();
		}

		T front()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.front();
		}
		
		T back()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.back();
		}

		void push_back(const T& item)
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			queue_.push_back(item);
		}

		void push_front(const T& item)
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.push_front(item);
		}

		bool empty()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.empty();
		}

		size_t count()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.size();
		}

		void clear()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.clear();
		}
		

		T pop_front()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			auto t = std::move(queue_.front());
			queue_.pop_front();
			return t;
		}


		T pop_back()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			auto t = std::move(queue_.back());
			queue_.pop_back();
			return t;
		}

		size_t size()
		{
			const std::lock_guard<std::mutex> lock(mutex_);
			return queue_.size();
		}


	protected:
		std::mutex mutex_;
		std::deque<T> queue_;
	};

}