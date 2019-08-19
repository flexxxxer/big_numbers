#ifndef CONCURRENCY_QUEUE_H
#define CONCURRENCY_QUEUE_H

#include <utility>
#include <atomic>

namespace simple_concurrency
{
	template <typename Q>
	struct node {
		node<Q>* next_;
		Q value_;

		explicit node(Q data ) : next_(nullptr), value_(std::move(data))
		{ }
	};
	
	template <typename T>
	class concurrency_queue  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
	{
		std::atomic<node<T>*> head_;
		std::atomic<node<T>*> tail_;
	public:
		concurrency_queue() : head_(nullptr), tail_(nullptr) {}

		[[nodiscard]] bool empty() const
		{
			return this->head_.load(std::memory_order_relaxed) == nullptr;
		}

		void concurrent_push(const T& val)
		{
			node<T>* new_node = new node<T>(val);

			// put the current value of head into new_node->next
			new_node->next_ = this->head_.load(std::memory_order_relaxed);

			// now make new_node the new head, but if the head
			// is no longer what's stored in new_node->next
			// (some other thread must have inserted a node just now)
			// then put that new head into new_node->next and try again
			while (!head_.compare_exchange_weak(new_node->next_, new_node,
				std::memory_order_release,
				std::memory_order_relaxed));
		}

		T* concurrent_pop() noexcept
		{
			node<T>* old_front = this->head_;
			node<T>* new_front;

			do {
				if (!old_front) 
					return nullptr; // nothing to pop
				new_front = old_front->next_;
			} while (!this->head_.compare_exchange_weak(old_front, new_front));

			// if the old front was also the back, the queue is now empty.
			new_front = old_front;
			if (this->tail_.compare_exchange_strong(new_front, nullptr))
				old_front->next_ = old_front;

			T value = old_front->value_;
			delete old_front;
			
			return new T(value);
		}

		~concurrency_queue()
		{
			node<T>* front = this->head_;

			while (front != nullptr)
			{
				node<T>* next = front->next_;
				delete front;
				front = next;
			}
		}
	};
}

#endif