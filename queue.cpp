#include "tbb/task_scheduler_init.h"
#include <atomic>
#include <boost/range/irange.hpp>
#include <chrono>
#include <execution>
#include <iomanip>
#include <iostream>

template <class T> class Node {
  // Node class representing a node of a linked list consiisting of a value data
  // field and a pointer to the next node.
public:
  T value;
  Node *next;
  std::atomic<Node<T> *> next1;

  // Constructor of the Node Class. Initializing the next pointer to nullptr.
  Node(T value) {
    this->value = value;
    // this->next1;
    this->next = nullptr;
  }
};

template <class T> class Lock_Free_Queue {
  // class to implement a lock free queue.
  // It has to pointers similar to a standard queue: A head and a tail both of
  // which are defined to be atomic Nodes so that they can call the atomic
  // compare_exchange function.
public:
  std::atomic<Node<T> *> head;
  std::atomic<Node<T> *> tail;
  Node<T> *p;

  // Initializing the Lock free queue with a given sentinel using an initializer
  // list
  Lock_Free_Queue(Node<T> *sentinel)
      : head(sentinel), tail(sentinel), p(sentinel)

  {}

  void enqueue(T value) {
    // initializing  the Node to be added
    Node<T> *temp = new Node<T>(value);
    // std::atomic<Node<T> *> t(temp);

    // Unless the current node is finally able to append itself it should keep
    // on trying. The loop will continue untill Nodes other than the current are
    // appended to the queue. As soon as the current value is appended the
    // function called returns.
    while (true) {
      // Define a pointer to the tail node in the current fucntion
      // std::atomic<Node<T> *> last(tail.load(std::memory_order_relaxed));
      Node<T> *last = tail.load(std::memory_order_relaxed);
      // std::cout<<"1"<<"\n";
      // if no other Node appended itself during this time.
      if (last == tail.load(std::memory_order_relaxed)) {

        // if this is not the case then the tail pointer at the wrong node.
        if (last->next1.load(std::memory_order_relaxed) == nullptr) {
          // atomically adding the node to be appended to the linked list

          if (last->next1.compare_exchange_strong(temp->next, temp)) {
            // comparing head with tail to check the base case which is enquing
            // the first node in our queue. So head->next needs to be updated.

            if (last == head.load(std::memory_order_relaxed)) {
              // Node<T> *a = last.load(std::memory_order_relaxed);
              tail.compare_exchange_strong(last, temp);

              head.compare_exchange_strong(p, temp);
            }
            // updating the tail pointer and as the node is appended so function
            // returns
            // Node<T> *b = last.load(std::memory_order_relaxed);
            tail.compare_exchange_strong(last, temp);

            return;
          }

        } else {
          // As the tail pointer was not at the correct node so it needs to be
          // updated. comparing the tail pointer with the last to check if no
          // new node is appended and then updating it to the actual last
          // position

          // Node<T> *c = last.load(std::memory_order_relaxed);`
          tail.compare_exchange_strong(
              last, last->next1.load(std::memory_order_relaxed));
        }
      }
    }
  }

  T deque() {

    // looping untill the node is dequed
    while (true) {
      // std::atomic<Node<T> *> first(head.load(std::memory_order_relaxed));
      Node<T> *first = head.load(std::memory_order_relaxed);
      // std::atomic<Node<T> *> next(first->next);
      Node<T> *f_next = first->next;
      // std::atomic<Node<T> *> last(tail.load(std::memory_order_relaxed));
      Node<T> *last = tail.load(std::memory_order_relaxed);
      Node<T> *p1;
      // Checking the empty queue case
      if (first == last) {
        // the case for the empty queue
        if (first == p)
          throw std::underflow_error("called deque on an empty queue");
        else if (f_next == nullptr) {
          Node<T> *a = head.load(std::memory_order_relaxed);
          head.compare_exchange_strong(a, p);
          Node<T> *b = tail.load(std::memory_order_relaxed);
          tail.compare_exchange_strong(b, p);

          tail.load(std::memory_order_relaxed)
              ->next1.compare_exchange_strong(a, nullptr);
          return first->value;
        } else {
          // tail pointer was incorrectly placed.
          // Node<T> *a = last.load(std::memory_order_relaxed);
          tail.compare_exchange_strong(
              last, head.load(std::memory_order_relaxed)
                        ->next1.load(std::memory_order_relaxed));
        }
      } else {

        // if head is same as first i.e. no new node appended then just update
        // the head pointer and return.
        // Node<T> *b = first.load(std::memory_order_relaxed);

        if (head.compare_exchange_strong(
                first, head.load(std::memory_order_relaxed)
                           ->next1.load(std::memory_order_relaxed))) {
          return first->value;
        }
      }
    }
    return 1;
  }
};

void printQ(Lock_Free_Queue<int> &q) {
  int count = 0;

  Node<int> *t = q.head.load(std::memory_order_relaxed);
  while (t != nullptr) {
    std::cout << t->value << " ";
    count++;
    t = t->next1.load(std::memory_order_relaxed);
  }
  std::cout << "\n";
  std::cout << "No of elements -> " << count << "\n";
}

Node<int> *sentinal = new Node<int>(-1);
Lock_Free_Queue<int> q(sentinal);

void enqueuer(int i) { q.enqueue(i); }

void dequer(int i) { q.deque(); }

int main(int argc, char *argv[]) {

  // defining a sentinel node for the queue and iniitializing a lock free queue

  // testing the queue
  int cores = std::atoi(argv[1]);
  tbb::task_scheduler_init init(cores);
  auto range = boost::irange(1, 65535);

  auto start = std::chrono::high_resolution_clock::now();
  std::for_each(std::execution::par, std::begin(range), std::end(range),
                enqueuer);
  auto end = std::chrono::high_resolution_clock::now();
  double time_taken =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  time_taken *= 1e-9;

  std::cout << "Time : " << std::fixed << time_taken << std::setprecision(9);
  std::cout << " sec";
}
