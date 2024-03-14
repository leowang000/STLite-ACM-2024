#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
public:
  priority_queue() : root_(nullptr), size_(0) {}

  priority_queue(const priority_queue &other) : size_(other.size_), root_(CopyTree(other.root_)) {}

  ~priority_queue() {
    ClearTree(root_);
  }

  priority_queue &operator=(const priority_queue &other) {
    if (this == &other) {
      return *this;
    }
    ClearTree(root_);
    size_ = other.size_;
    root_ = CopyTree(other.root_);
    return *this;
  }

  /**
   * get the top of the queue.
   * @return a reference of the top element.
   * throw container_is_empty if empty() returns true;
   */
  const T &top() const {
    if (size_ == 0) {
      throw container_is_empty();
    }
    return root_->data_;
  }

  /**
   * push new element to the priority queue.
   */
  void push(const T &e) {
    Node *new_node = new Node(e);
    try {
      root_ = MergeTree(root_, new_node);
      size_++;
    } catch (...) {
      delete new_node;
    }
  }

  /**
   * delete the top element.
   * throw container_is_empty if empty() returns true;
   */
  void pop() {
    if (size_ == 0) {
      throw container_is_empty();
    }
    Node *new_root = MergeTree(root_->left_, root_->right_);
    delete root_;
    root_ = new_root;
    size_--;
  }

  /**
   * return the number of the elements.
   */
  size_t size() const {
    return size_;
  }

  /**
   * check if the container has at least an element.
   * @return true if it is empty, false if it has at least an element.
   */
  bool empty() const {
    return size_ == 0;
  }

  /**
   * merge two priority_queues with at most O(logn) complexity.
   * clear the other priority_queue.
   */
  void merge(priority_queue &other) {
    root_ = MergeTree(root_, other.root_);
    other.root_ = nullptr;
    size_ += other.size_;
    other.size_ = 0;
  }

private:
  struct Node {
    T data_;
    Node *left_, *right_;

    explicit Node(const T &data, Node *left = nullptr, Node *right = nullptr)
        : data_(data), left_(left), right_(right) {}
  };

  static Node *CopyTree(const Node *root) {
    if (root == nullptr) {
      return nullptr;
    }
    return new Node(root->data_, CopyTree(root->left_), CopyTree(root->right_));
  }

  static void ClearTree(const Node *root) {
    if (root == nullptr) {
      return;
    }
    ClearTree(root->left_);
    ClearTree(root->right_);
    delete root;
  }

  static Node *MergeTree(Node *root1, Node *root2) {
    if (root1 == nullptr) {
      return root2;
    }
    if (root2 == nullptr) {
      return root1;
    }
    if (Compare()(root1->data_, root2->data_)) {
      Node *new_root = MergeTree(root1, root2->right_);
      root2->right_ = root2->left_;
      root2->left_ = new_root;
      return root2;
    }
    Node *new_root = MergeTree(root1->right_, root2);
    root1->right_ = root1->left_;
    root1->left_ = new_root;
    return root1;
  }

  Node *root_;
  size_t size_;
};

}

#endif
