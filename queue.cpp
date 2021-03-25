#include <atomic>
#include <iostream>

template <class T> class Node {
  // Node class representing a node of a linked list consiisting of a value data
  // field and a pointer to the next node.
public:
  T value;
  Node *next;

  // Constructor of the Node Class. Initializing the next pointer to Null.
  Node(T value) {
    this->value = value;
    this->next = NULL;
  }
};
// template <class _Tp>
// std::atomic<_Tp*>& std::atomic<_Tp*>::operator=(const std::atomic<_Tp*>&
// head){ return head;
// }
template <class T>
bool compare_and_exchange(std::atomic<Node<T> *> &mem, Node<T> *cmp,
                          Node<T> *with) {
  // atomic compare and exchange function using the inbuilt
  // compare_exchage_strong function in atomic class in C++
  return mem.compare_exchange_strong(cmp, with, std::memory_order_release,
                                     std::memory_order_relaxed);
}
template <class T>
bool compare_and_exchange_new(Node<T> **mem, Node<T> *cmp, Node<T> *with) {
  // atomic compare and exchange function using the inbuilt
  // compare_exchage_strong function in atomic class in C++
  std::atomic<Node<T> *> temp(*mem);
  if (temp.compare_exchange_strong(cmp, with, std::memory_order_release,
                                   std::memory_order_relaxed)) {
    *mem = temp.load(std::memory_order_relaxed);
    return true;
  }
  return false;
}

template <class T> class Lock_Free_Queue {
  // class to implement a lock free queue.
  // It has to pointers similar to a standard queue: A head and a tail both of
  // which are defined to be atomic Nodes so that they can call the atomic
  // compare_exchange function.
public:
  std::atomic<Node<T> *> head;
  std::atomic<Node<T> *> tail;

  // A default constructor

  Lock_Free_Queue();
  // Initializing the Lock free queue with a given sentinel using an initializer
  // list
  Lock_Free_Queue(Node<T> *sentinel)
      : head(sentinel), tail(sentinel)

  {}
  Lock_Free_Queue(const Lock_Free_Queue &q) {
    head = q.head;
    tail = q.tail;
  }
  void enqueue(T value) {
    // initializing  the Node to be added
    Node<T> *t = new Node<T>(value);

    // Unless the current node is finally able to append itself it should keep
    // on trying. The loop will continue untill Nodes other than the current are
    // appended to the queue. As soon as the current value is appended the
    // function called returns.
    while (true) {
      // Define a pointer to the tail node in the current fucntion
      Node<T> *last = tail;
      Node<T> *next = last->next;

      // if no other Node appended itself during this time.
      if (last == tail) {
        // if this is not the case then the tail pointer at the wrong node.
        if (next == NULL) {
          // atomically adding the node to be appended to the linked list
          if (compare_and_exchange_new<T>(&last->next, NULL, t)) {
            // comparing head with tail to check the base case which is enquing
            // the first node in our queue. So head->next needs to be updated.
            if (tail == head) {
              compare_and_exchange(tail, last, t);
              compare_and_exchange_new<T>(
                  &head.load(std::memory_order_relaxed)->next, NULL, last);
            }
            // updating the tail pointer and as the node is appended so function
            // returns
            compare_and_exchange(tail, last, t);
            // printng the first(node after the sentinel node) value and tail
            // value after each enqueue operation.
            // just for testing purpose, will be removed later.
            std::cout << head.load(std::memory_order_relaxed)->next->value
                      << " ";
            std::cout << tail.load(std::memory_order_relaxed)->value
                      << std::endl;

            return;
          }

        } else {
          // As the tail pointer was not at the correct node so it needs to be
          // updated. comparing the tail pointer with the last to check if no
          // new node is appended and then updating it to the actual last
          // position
          compare_and_exchange(tail, last, next);
        }
      }
    }
  }

  T deque() {

    // looping untill the node is dequed
    while (true) {
      Node<T> *first = head;
      Node<T> *next = first->next;
      Node<T> *last = tail;

      // Checking the empty queue case
      if (first == last) {
        // the case for the empty queue
        if (next == NULL)

          throw std::underflow_error("called deque on an empty queue");

        else {
          // tail pointer was incorrectly placed.
          compare_and_exchange(tail, last, next);
        }
      } else {
        // the value to be dequed will be returned by the fucntion
        T value = first->value;

        // if head is same as first i.e. no new node appended then just update
        // the head pointer and return.
        if (compare_and_exchange(head, first, next)) {
          return value;
        }
      }
    }
    return 1;
  }
};
int main() {

  // defining a sentinel node for the queue and iniitializing a lock free queue
  Node<int> *sentinal = new Node<int>(-1);
  Lock_Free_Queue<int> q(sentinal);

  // testing the queue
  q.enqueue(10);
  q.enqueue(20);
  q.enqueue(30);

   
}
