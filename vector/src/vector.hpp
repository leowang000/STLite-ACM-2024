#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <cstring>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T, typename Allocator = std::allocator<T>>
class vector {
public:
  /**
   * a type for actions of the elements of a vector, and you should write
   *   a class named const_iterator with same interfaces.
   */
  /**
   * you can see RandomAccessIterator at CppReference for help.
   */
  class const_iterator;

  class iterator {
    // The following code is written for the C++ type_traits library.
    // Type traits is a C++ feature for describing certain properties of a type.
    // For instance, for an iterator, iterator::value_type is the type that the
    // iterator points to.
    // STL algorithms and containers may use these type_traits (e.g. the following
    // typedef) to work properly. In particular, without the following code,
    // @code{std::sort(iter, iter1);} would not compile.
    // See these websites for more information:
    // https://en.cppreference.com/w/cpp/header/type_traits
    // About value_type: https://blog.csdn.net/u014299153/article/details/72419713
    // About iterator_category: https://en.cppreference.com/w/cpp/iterator
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

  private:
    T *data_;
    const vector<T> *vec_ptr_;

  public:
    iterator(T *data, const vector<T> *vec_ptr) : data_(data), vec_ptr_(vec_ptr) {}

    iterator(const iterator &other) : data_(other.data_), vec_ptr_(other.vec_ptr_) {}

    iterator &operator=(const iterator &other) = default;

    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    iterator operator+(const int &n) const {
      return {data_ + n, vec_ptr_};
    }

    iterator operator-(const int &n) const {
      return {data_ - n, vec_ptr_};
    }

    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invaild_iterator.
    int operator-(const iterator &rhs) const {
      if (vec_ptr_ != rhs.vec_ptr_) {
        throw invalid_iterator();
      }
      return data_ - rhs.data_;
    }

    iterator &operator+=(const int &n) {
      data_ += n;
      return *this;
    }

    iterator &operator-=(const int &n) {
      data_ -= n;
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      ++data_;
      return old;
    }

    iterator &operator++() {
      ++data_;
      return *this;
    }

    iterator operator--(int) {
      iterator old = *this;
      --data_;
      return old;
    }

    iterator &operator--() {
      --data_;
      return *this;
    }

    T &operator*() const {
      return *data_;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return data_ == rhs.data_;
    }

    bool operator==(const const_iterator &rhs) const {
      return data_ == rhs.data_;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return data_ != rhs.data_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return data_ != rhs.data_;
    }
  };

  /**
   * has same function as iterator, just for a const object.
   */
  class const_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = std::output_iterator_tag;

  private:
    const T *data_;
    const vector<T> *vec_ptr_;

  public:
    const_iterator(const T *data, const vector<T> *vec_ptr) : data_(data), vec_ptr_(vec_ptr) {}

    const_iterator(const iterator &other) : data_(other.data_), vec_ptr_(other.vec_ptr_) {}

    const_iterator &operator=(const const_iterator &other) = default;

    /**
     * return a new iterator which pointer n-next elements
     * as well as operator-
     */
    const_iterator operator+(const int &n) const {
      return {data_ + n, vec_ptr_};
    }

    const_iterator operator-(const int &n) const {
      return {data_ - n, vec_ptr_};
    }

    // return the distance between two iterators,
    // if these two iterators point to different vectors, throw invaild_iterator.
    int operator-(const const_iterator &rhs) const {
      if (vec_ptr_ != rhs.vec_ptr_) {
        throw invalid_iterator();
      }
      return data_ - rhs.data_;
    }

    const_iterator &operator+=(const int &n) {
      data_ += n;
      return *this;
    }

    const_iterator &operator-=(const int &n) {
      data_ -= n;
      return *this;
    }

    const_iterator operator++(int) {
      iterator old = *this;
      ++data_;
      return old;
    }

    const_iterator &operator++() {
      ++data_;
      return *this;
    }

    const_iterator operator--(int) {
      iterator old = *this;
      --data_;
      return old;
    }

    const_iterator &operator--() {
      --data_;
      return *this;
    }

    const T &operator*() const {
      return *data_;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory address).
     */
    bool operator==(const iterator &rhs) const {
      return data_ == rhs.data_;
    }

    bool operator==(const const_iterator &rhs) const {
      return data_ == rhs.data_;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return data_ != rhs.data_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return data_ != rhs.data_;
    }
  };

  /**
   * Constructors
   * At least two: default constructor, copy constructor
   */
  vector() : alloc_(), first_(alloc_.allocate(minCapacity)), size_(0), capacity_(minCapacity) {}

  vector(const vector &other) : alloc_() {
    first_ = alloc_.allocate(other.capacity_);
    size_ = other.size_;
    capacity_ = other.capacity_;
    Copy(first_, other.first_, other.size_);
  }

  /**
   * Destructor
   */
  ~vector() {
    Release();
  }

  /**
   * Assignment operator
   */
  vector &operator=(const vector &other) {
    if (this == &other) {
      return *this;
    }
    Release();
    first_ = alloc_.allocate(other.capacity_);
    size_ = other.size_;
    capacity_ = other.capacity_;
    Copy(first_, other.first_, other.size_);
    return *this;
  }

  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   */
  T &at(const size_t &pos) {
    if (pos >= size_) {
      throw index_out_of_bound();
    }
    return first_[pos];
  }

  const T &at(const size_t &pos) const {
    if (pos >= size_) {
      throw index_out_of_bound();
    }
    return first_[pos];
  }

  /**
   * assigns specified element with bounds checking
   * throw index_out_of_bound if pos is not in [0, size)
   * !!! Pay attentions
   *   In STL this operator does not check the boundary but I want you to do.
   */
  T &operator[](const size_t &pos) {
    if (pos >= size_) {
      throw index_out_of_bound();
    }
    return first_[pos];
  }

  const T &operator[](const size_t &pos) const {
    if (pos >= size_) {
      throw index_out_of_bound();
    }
    return first_[pos];
  }

  /**
   * access the first element.
   * throw container_is_empty if size == 0
   */
  const T &front() const {
    if (size_ == 0) {
      throw container_is_empty();
    }
    return first_[0];
  }

  /**
   * access the last element.
   * throw container_is_empty if size == 0
   */
  const T &back() const {
    if (size_ == 0) {
      throw container_is_empty();
    }
    return first_[size_ - 1];
  }

  /**
   * returns an iterator to the beginning.
   */
  iterator begin() {
    return {first_, this};
  }

  const_iterator cbegin() const {
    return {first_, this};
  }

  /**
   * returns an iterator to the end.
   */
  iterator end() {
    return {first_ + size_, this};
  }

  const_iterator cend() const {
    return {first_ + size_, this};
  }

  /**
   * checks whether the container is empty
   */
  bool empty() const {
    return size_ == 0;
  }

  /**
   * returns the number of elements
   */
  size_t size() const {
    return size_;
  }

  /**
   * clears the contents
   */
  void clear() {
    Release();
    first_ = alloc_.allocate(minCapacity);
    size_ = 0;
    capacity_ = minCapacity;
  }

  /**
   * inserts value before pos
   * returns an iterator pointing to the inserted value.
   */
  iterator insert(iterator pos, const T &value) {
    return insert(pos - begin(), value);
  }

  /**
   * inserts value at index ind.
   * after inserting, this->at(ind) == value
   * returns an iterator pointing to the inserted value.
   * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
   */
  iterator insert(const size_t &ind, const T &value) {
    if (ind > size_) {
      throw index_out_of_bound();
    }
    if (size_ < capacity_ - 1) {
      new (first_ + size_) T(value);
      for (int i = size_; i > ind; i--) {
        first_[i] = first_[i - 1];
      }
      first_[ind] = value;
      size_++;
    } else {
      T *new_space = alloc_.allocate(2 * capacity_);
      Copy(new_space, first_, ind);
      new(new_space + ind) T(value);
      Copy(new_space + ind + 1, first_ + ind, size_ - ind);
      Release();
      first_ = new_space;
      size_++;
      capacity_ *= 2;
    }
    return {first_ + ind, this};
  }

  /**
   * removes the element at pos.
   * return an iterator pointing to the following element.
   * If the iterator pos refers the last element, the end() iterator is returned.
   */
  iterator erase(iterator pos) {
    return erase(pos - begin());
  }

  /**
   * removes the element with index ind.
   * return an iterator pointing to the following element.
   * throw index_out_of_bound if ind >= size
   */
  iterator erase(const size_t &ind) {
    if (ind >= size_) {
      throw index_out_of_bound();
    }
    if (capacity_ > minCapacity && size_ + 1 <= capacity_ / 4) {
      size_t new_capacity = std::max(capacity_ / 4, minCapacity);
      T *new_space = alloc_.allocate(new_capacity);
      Copy(new_space, first_, ind);
      Copy(new_space + ind, first_ + ind + 1, size_ - 1 - ind);
      Release();
      first_ = new_space;
      size_--;
      capacity_ = new_capacity;
    } else {
      for (int i = ind; i + 2 <= size_; i++) {
        new(first_ + i) T(first_[i + 1]);
      }
      (first_ + size_ - 1)->~T();
      size_--;
    }
    return {first_ + ind, this};
  }

  /**
   * adds an element to the end.
   */
  void push_back(const T &value) {
    insert(size_, value);
  }

  /**
   * remove the last element from the end.
   * throw container_is_empty if size() == 0
   */
  void pop_back() {
    if (size_ == 0) {
      throw container_is_empty();
    }
    erase(size_ - 1);
  }

private:
  static const size_t minCapacity;

  Allocator alloc_;
  T *first_;
  size_t size_, capacity_;

  void Release() {
    for (int i = 0; i < size_; i++) {
      (first_ + i)->~T();
    }
    alloc_.deallocate(first_, capacity_);
  }
  void Copy(T *dest, T *src, size_t n) {
    for (int i = 0; i < n; i++) {
      new(dest + i) T(src[i]);
    }
  }
};

template<typename T, typename Allocator> const size_t vector<T, Allocator>::minCapacity = 8;

}

#endif
