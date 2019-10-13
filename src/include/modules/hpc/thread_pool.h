#pragma once

#include <thread>
#include <future>

#include <vector>
#include <functional>
#include <mutex>

#include "concurrency/concurrency_queue.h"

namespace hpc
{

	template <typename TasksRetT>
	class thread_pool
	{
		struct thread_util
		{
			static void relax_and_wait_job(thread_pool* thp)
			{
				while (true)
				{
					{
						std::unique_lock<std::mutex> lock(thp->locker_);
						thp->condition_.wait(lock, [thp]()
						{
							return !thp->tasks_queue_.empty() || thp->is_in_closing_;
						});

						if (thp->is_in_closing_)
							return;
					}

					std::pair<std::function<TasksRetT()>, std::promise<TasksRetT>*>* task = thp->tasks_queue_.concurrent_pop();

					if(task == nullptr)
						continue;

					task->second->set_value(task->first());
					delete task->second;
					delete task;

					thp->condition_.notify_one();
				}
			}

			static uint32_t next_power_of_2(uint32_t n)
			{
				const auto tmp = n;
				
				n--;
				n |= n >> 1;
				n |= n >> 2;
				n |= n >> 4;
				n |= n >> 8;
				n |= n >> 16;
				n++;

				// maybe this is faster way for 3, 6, 10 and etc cores, but i don't now
				//if (tmp < n)
					//n >>= 1;
				
				return n;
			}
		};

		std::vector<std::thread> threads_; // threads for performing
		uint32_t threads_count_{}; // not edit in methods

		simple_concurrency::concurrency_queue<std::pair<std::function<TasksRetT()>, std::promise<TasksRetT>*>> tasks_queue_;
		
		std::condition_variable condition_;
		std::mutex locker_;

		bool is_in_closing_ = false;

		explicit thread_pool(const uint32_t threads_capacity = std::thread::hardware_concurrency())
		{
			// init base
			this->threads_count_ = thread_util::next_power_of_2(threads_capacity);
			this->threads_.resize(threads_count_);

			// init threads
			for (uint32_t i = 0; i < threads_count_; i++)
				this->threads_[i] = std::thread(&thread_util::relax_and_wait_job, this);
		}

	public:

		static thread_pool<TasksRetT>& get_instance()
		{
			static thread_pool<TasksRetT> instance;

			return instance;
		}

		[[nodiscard]] uint32_t threads_capacity() const
		{
			return this->threads_count_;
		}

		std::future<TasksRetT> run(std::function<TasksRetT()> func)
		{
			std::unique_lock<std::mutex> lock(this->locker_);
			std::promise<TasksRetT>* p = new std::promise<TasksRetT>();

			this->tasks_queue_.concurrent_push(std::make_pair(func, p));

			this->condition_.notify_all();

			return (*p).get_future();
		}

		void wait_all_jobs()
		{
			{
				std::unique_lock<std::mutex> lock(this->locker_);
				this->condition_.wait(lock, [this]()
				{
					return this->tasks_queue_.empty();
				});
			}
		}

		~thread_pool()
		{
			this->is_in_closing_ = true;
			this->condition_.notify_all();
			
			for (uint32_t i = 0; i < threads_count_; i++)
				this->threads_[i].join();

			tasks_queue_.~concurrency_queue();
		}
	};
}