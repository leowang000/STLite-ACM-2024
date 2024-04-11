/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
enum Colour {
  kRed, kBlack
};

template<class Key, class T, class KeyCompare = std::less<Key>>
class map {
private:
  struct RBTreeNode {
    Colour colour_;
    pair<const Key, T> data_;
    RBTreeNode *parent_, *left_, *right_;

    explicit RBTreeNode(const pair<const Key, T> &data, Colour colour, RBTreeNode *parent = nullptr,
                        RBTreeNode *left = nullptr, RBTreeNode *right = nullptr) :
        data_(data), colour_(colour), parent_(parent), left_(left), right_(right) {}
  };

  static void ClearTree(RBTreeNode *root) {
    if (root == nullptr) {
      return;
    }
    ClearTree(root->left_);
    ClearTree(root->right_);
    delete root;
  }

  static RBTreeNode *CopyTree(const RBTreeNode *other_root) {
    if (other_root == nullptr) {
      return nullptr;
    }
    RBTreeNode *now_root = new RBTreeNode(other_root->data_, other_root->colour_, nullptr, CopyTree(other_root->left_),
                                          CopyTree(other_root->right_));
    if (now_root->left_ != nullptr) {
      now_root->left_->parent_ = now_root;
    }
    if (now_root->right_ != nullptr) {
      now_root->right_->parent_ = now_root;
    }
    return now_root;
  }

  template<class PtrType>
  static PtrType GetMinimum(PtrType root) {
    PtrType res = root;
    while (res->left_ != nullptr) {
      res = res->left_;
    }
    return res;
  }

  template<class PtrType>
  static PtrType GetMaximum(PtrType root) {
    PtrType res = root;
    while (res->right_ != nullptr) {
      res = res->right_;
    }
    return res;
  }

  template<class PtrType>
  static PtrType FindNext(PtrType node) {
    if (node->right_ != nullptr) {
      return GetMinimum(node->right_);
    }
    while (node->parent_ != nullptr && node->parent_->right_ == node) {
      node = node->parent_;
    }
    return node->parent_;
  }

  template<class PtrType>
  static PtrType FindPrev(PtrType node) {
    if (node->left_ != nullptr) {
      return GetMaximum(node->left_);
    }
    while (node->parent_ != nullptr && node->parent_->left_ == node) {
      node = node->parent_;
    }
    return node->parent_;
  }

  void LL(RBTreeNode *&g) {
    RBTreeNode *g_par = g->parent_, *p = g->left_, *p_r = p->right_;
    p->right_ = g;
    g->parent_ = p;
    p->parent_ = g_par;
    g->left_ = p_r;
    if (p_r != nullptr) {
      p_r->parent_ = g;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = p;
      }
      else {
        g_par->right_ = p;
      }
    }
    else {
      root_ = p;
    }
  }

  void LLb(RBTreeNode *g) {
    LL(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  void RR(RBTreeNode *g) {
    RBTreeNode *g_par = g->parent_, *p = g->right_, *p_l = p->left_;
    p->left_ = g;
    g->parent_ = p;
    p->parent_ = g_par;
    g->right_ = p_l;
    if (p_l != nullptr) {
      p_l->parent_ = g;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = p;
      }
      else {
        g_par->right_ = p;
      }
    }
    else {
      root_ = p;
    }
  }

  void RRb(RBTreeNode *g) {
    RR(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  void LR(RBTreeNode *g) {
    RBTreeNode *g_par = g->parent_, *p = g->left_, *x = p->right_, *x_l = x->left_, *x_r = x->right_;
    g->parent_ = x;
    g->left_ = x_r;
    p->parent_ = x;
    p->right_ = x_l;
    x->parent_ = g_par;
    x->left_ = p;
    x->right_ = g;
    if (x_l != nullptr) {
      x_l->parent_ = p;
    }
    if (x_r != nullptr) {
      x_r->parent_ = g;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = x;
      }
      else {
        g_par->right_ = x;
      }
    }
    else {
      root_ = x;
    }
  }

  void LRb(RBTreeNode *g) {
    LR(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  void RL(RBTreeNode *g) {
    RBTreeNode *g_par = g->parent_, *p = g->right_, *x = p->left_, *x_l = x->left_, *x_r = x->right_;
    g->parent_ = x;
    g->right_ = x_l;
    p->parent_ = x;
    p->left_ = x_r;
    x->parent_ = g_par;
    x->left_ = g;
    x->right_ = p;
    if (x_l != nullptr) {
      x_l->parent_ = g;
    }
    if (x_r != nullptr) {
      x_r->parent_ = p;
    }
    if (g_par != nullptr) {
      if (g_par->left_ == g) {
        g_par->left_ = x;
      }
      else {
        g_par->right_ = x;
      }
    }
    else {
      root_ = x;
    }
  }

  void RLb(RBTreeNode *g) {
    RL(g);
    g->colour_ = kRed;
    g->parent_->colour_ = kBlack;
  }

  pair<RBTreeNode *, bool> InsertNode(const pair<const Key, T> &value) {
    if (root_ == nullptr) {
      root_ = new RBTreeNode(value, kBlack);
      return {root_, true};
    }
    RBTreeNode *now = root_, *res;
    while (true) {
      bool is_smaller = KeyCompare()(value.first, now->data_.first);
      bool is_bigger = KeyCompare()(now->data_.first, value.first);
      if (!is_smaller && !is_bigger) {
        return {now, false};
      }
      if (is_smaller && now->left_ == nullptr) {
        now->left_ = new RBTreeNode(value, kRed, now);
        res = now->left_;
        if (now->colour_ == kBlack) {
          break;
        }
        if (now->parent_->left_ == now) {
          LLb(now->parent_);
        }
        else {
          RLb(now->parent_);
        }
        break;
      }
      if (is_bigger && now->right_ == nullptr) {
        now->right_ = new RBTreeNode(value, kRed, now);
        res = now->right_;
        if (now->colour_ == kBlack) {
          break;
        }
        if (now->parent_->left_ == now) {
          LRb(now->parent_);
        }
        else {
          RRb(now->parent_);
        }
        break;
      }
      if (now->colour_ == kBlack && now->left_ != nullptr && now->left_->colour_ == kRed &&
          now->right_ != nullptr && now->right_->colour_ == kRed) {
        RBTreeNode *now_par = now->parent_;
        now->colour_ = kRed;
        if (now->left_ != nullptr) {
          now->left_->colour_ = kBlack;
        }
        if (now->right_ != nullptr) {
          now->right_->colour_ = kBlack;
        }
        if (now_par != nullptr && now_par->colour_ == kRed) {
          if (now_par->left_ == now) {
            if (now_par->parent_->left_ == now_par) {
              LLb(now_par->parent_);
            }
            else {
              RLb(now_par->parent_);
            }
          }
          else {
            if (now_par->parent_->left_ == now_par) {
              LRb(now_par->parent_);
            }
            else {
              RRb(now_par->parent_);
            }
          }
        }
      }
      now = (is_smaller ? now->left_ : now->right_);
    }
    root_->colour_ = kBlack;
    return {res, true};
  }

  void EraseNode(RBTreeNode *node) {
    if (size_ == 1) {
      root_ = nullptr;
      delete node;
      return;
    }
    RBTreeNode *now = root_;
    bool find_substitution = false;
    while (true) {
      while (true) {
        if (now->colour_ == kRed) {
          break;
        }
        RBTreeNode *&par = now->parent_;
        if ((now->left_ == nullptr || now->left_->colour_ == kBlack) &&
            (now->right_ == nullptr || now->right_->colour_ == kBlack)) {
          if (par == nullptr) {
            now->colour_ = kRed;
            break;
          }
          bool is_now_left_son = (par->left_ == now);
          RBTreeNode *sib = (is_now_left_son ? par->right_ : par->left_);
          if (sib == nullptr || ((sib->left_ == nullptr || sib->left_->colour_ == kBlack) &&
                                 (sib->right_ == nullptr || sib->right_->colour_ == kBlack))) {
            par->colour_ = kBlack;
            now->colour_ = kRed;
            if (sib != nullptr) {
              sib->colour_ = kRed;
            }
            break;
          }
          if (is_now_left_son) {
            if (sib->right_ != nullptr && sib->right_->colour_ == kRed) {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              sib->colour_ = kRed;
              sib->right_->colour_ = kBlack;
              RR(par);
              break;
            }
            else {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              RL(par);
              break;
            }
          }
          else {
            if (sib->left_ != nullptr && sib->left_->colour_ == kRed) {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              sib->colour_ = kRed;
              sib->left_->colour_ = kBlack;
              LL(par);
              break;
            }
            else {
              par->colour_ = kBlack;
              now->colour_ = kRed;
              LR(par);
              break;
            }
          }
        }
        if ((!find_substitution && now == node) || (find_substitution && now->left_ == nullptr)) {
          if (now->left_ != nullptr && now->right_ != nullptr) {
            if (now->right_->colour_ == kRed) {
              break;
            }
            LLb(now);
            break;
          }
          if (now->left_ == nullptr) {
            RRb(now);
          }
          else {
            LLb(now);
          }
          break;
        }
        bool is_smaller = (find_substitution || KeyCompare()(node->data_.first, now->data_.first));
        now = (is_smaller ? now->left_ : now->right_);
        if (now->colour_ == kRed) {
          break;
        }
        if (is_smaller) {
          RRb(now->parent_);
        }
        else {
          LLb(now->parent_);
        }
      }
      RBTreeNode *&par = now->parent_;
      bool is_now_left_son = (par != nullptr && par->left_ == now);
      if (now == node) {
        if (now->left_ == nullptr && now->right_ == nullptr) {
          if (par == nullptr) {
            break;
          }
          if (is_now_left_son) {
            par->left_ = nullptr;
          }
          else {
            par->right_ = nullptr;
          }
          break;
        }
        if (now->left_ == nullptr) {
          now->right_->parent_ = par;
          if (par == nullptr) {
            break;
          }
          if (is_now_left_son) {
            par->left_ = now->right_;
          }
          else {
            par->right_ = now->right_;
          }
          break;
        }
        if (now->right_ == nullptr) {
          now->left_->parent_ = par;
          if (par == nullptr) {
            break;
          }
          if (is_now_left_son) {
            par->left_ = now->left_;
          }
          else {
            par->right_ = now->left_;
          }
          break;
        }
        find_substitution = true;
        now = now->right_;
        continue;
      }
      if (find_substitution && now->left_ == nullptr) {
        if (node->left_ != nullptr) {
          node->left_->parent_ = now;
        }
        if (now == node->right_) {
          if (node->parent_ != nullptr) {
            if (node->parent_->left_ == node) {
              node->parent_->left_ = now;
            }
            else {
              node->parent_->right_ = now;
            }
          }
          else {
            root_ = now;
          }
          par = node->parent_;
          now->left_ = node->left_;
          now->colour_ = node->colour_;
          break;
        }
        par->left_ = nullptr;
        node->right_->parent_ = now;
        if (node->parent_ != nullptr) {
          if (node->parent_->left_ == node) {
            node->parent_->left_ = now;
          }
          else {
            node->parent_->right_ = now;
          }
        }
        else {
          root_ = now;
        }
        par = node->parent_;
        now->left_ = node->left_;
        now->right_ = node->right_;
        now->colour_ = node->colour_;
        break;
      }
      now = (find_substitution || KeyCompare()(node->data_.first, now->data_.first) ? now->left_ : now->right_);
    }
    root_->colour_ = kBlack;
    delete node;
  }

  RBTreeNode *FindNode(const Key &key) const {
    RBTreeNode *now = root_;
    while (now != nullptr) {
      bool is_smaller = KeyCompare()(key, now->data_.first), is_bigger = KeyCompare()(now->data_.first, key);
      if (!is_smaller && !is_bigger) {
        return now;
      }
      now = (is_smaller ? now->left_ : now->right_);
    }
    return nullptr;
  }

  RBTreeNode *root_, *min_node_, *max_node_;
  size_t size_;

public:
  /**
   * the internal type of data.
   * it should have a default constructor, a copy constructor.
   * You can use sjtu::map as value_type by typedef.
   */
  typedef pair<const Key, T> value_type;

  /**
   * see BidirectionalIterator at CppReference for help.
   *
   * if there is anything wrong throw invalid_iterator.
   *     like it = map.begin(); --it;
   *       or it = map.end(); ++end();
   */
  class const_iterator;

  class iterator {
  private:
    friend map<Key, T, KeyCompare>;

    iterator(RBTreeNode *node, map<Key, T, KeyCompare> *map_ptr) : node_(node), map_ptr_(map_ptr) {}

    RBTreeNode *node_;
    const map<Key, T, KeyCompare> *map_ptr_;
    /**
     * add data members
     *   just add whatever you want.
     */
  public:
    iterator() : node_(nullptr), map_ptr_(nullptr) {}

    iterator(const iterator &other) : node_(other.node_), map_ptr_(other.map_ptr_) {}

    /**
     * iter++
     */
    iterator operator++(int) {
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      iterator old = *this;
      node_ = FindNext(node_);
      return old;
    }

    /**
     * ++iter
     */
    iterator &operator++() {
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      node_ = FindNext(node_);
      return *this;
    }

    /**
     * iter--
     */
    iterator operator--(int) {
      if (node_ == nullptr) {
        if (map_ptr_->root_ == nullptr) {
          throw invalid_iterator();
        }
        node_ = map_ptr_->max_node_;
        return *this;
      }
      iterator old = *this;
      node_ = FindPrev(node_);
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      return old;
    }

    /**
     * --iter
     */
    iterator &operator--() {
      if (node_ == nullptr) {
        if (map_ptr_->root_ == nullptr) {
          throw invalid_iterator();
        }
        node_ = map_ptr_->max_node_;;
        return *this;
      }
      node_ = FindPrev(node_);
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    value_type &operator*() const {
      return node_->data_;
    }

    bool operator==(const iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    bool operator==(const const_iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    value_type *operator->() const noexcept {
      return &(node_->data_);
    }
  };

  class const_iterator {
    // it should have similar member method as iterator.
    //  and it should be able to construct from an iterator.
  private:
    friend map<Key, T, KeyCompare>;

    const_iterator(RBTreeNode *node, const map<Key, T, KeyCompare> *map_ptr) : node_(node), map_ptr_(map_ptr) {}

    const RBTreeNode *node_;
    const map<Key, T, KeyCompare> *map_ptr_;
    // data members.
  public:
    const_iterator() : node_(nullptr) {}

    const_iterator(const const_iterator &other) : node_(other.node_) {}

    const_iterator(const iterator &other) : node_(other.node_) {}

    /**
     * iter++
     */
    const_iterator operator++(int) {
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      const_iterator old = *this;
      node_ = FindNext(node_);
      return old;
    }

    /**
     * ++iter
     */
    const_iterator &operator++() {
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      node_ = FindNext(node_);
      return *this;
    }

    /**
     * iter--
     */
    const_iterator operator--(int) {
      if (node_ == nullptr) {
        if (map_ptr_->root_ == nullptr) {
          throw invalid_iterator();
        }
        node_ = map_ptr_->max_node_;
        return *this;
      }
      const_iterator old = *this;
      node_ = FindPrev(node_);
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      return old;
    }

    /**
     * --iter
     */
    const_iterator &operator--() {
      if (node_ == nullptr) {
        if (map_ptr_->root_ == nullptr) {
          throw invalid_iterator();
        }
        node_ = map_ptr_->max_node_;
        return *this;
      }
      node_ = FindPrev(node_);
      if (node_ == nullptr) {
        throw invalid_iterator();
      }
      return *this;
    }

    /**
     * a operator to check whether two iterators are same (pointing to the same memory).
     */
    const value_type &operator*() const {
      return node_->data_;
    }

    bool operator==(const iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    bool operator==(const const_iterator &rhs) const {
      return map_ptr_ == rhs.map_ptr_ && node_ == rhs.node_;
    }

    /**
     * some other operator for iterator.
     */
    bool operator!=(const iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    bool operator!=(const const_iterator &rhs) const {
      return map_ptr_ != rhs.map_ptr_ || node_ != rhs.node_;
    }

    /**
     * for the support of it->first.
     * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
     */
    const value_type *operator->() const noexcept {
      return &(node_->data_);
    }
  };

  /**
   * two constructors
   */
  map() : size_(0), root_(nullptr), min_node_(nullptr), max_node_(nullptr) {}

  map(const map &other) : size_(other.size_) {
    root_ = CopyTree(other.root_);
    min_node_ = (root_ == nullptr ? nullptr : GetMinimum(root_));
    max_node_ = (root_ == nullptr ? nullptr : GetMaximum(root_));
  }

  /**
   * assignment operator
   */
  map &operator=(const map &other) {
    if (this == &other) {
      return *this;
    }
    ClearTree(root_);
    root_ = CopyTree(other.root_);
    min_node_ = (root_ == nullptr ? nullptr : GetMinimum(root_));
    max_node_ = (root_ == nullptr ? nullptr : GetMaximum(root_));
    size_ = other.size_;
    return *this;
  }

  /**
   * Destructors
   */
  ~map() {
    ClearTree(root_);
  }

  /**
   * access specified element with bounds checking
   * Returns a reference to the mapped value of the element with key equivalent to key.
   * If no such element exists, an exception of type `index_out_of_bound'
   */
  T &at(const Key &key) {
    RBTreeNode *res = FindNode(key);
    if (res == nullptr) {
      throw index_out_of_bound();
    }
    return res->data_.second;
  }

  const T &at(const Key &key) const {
    RBTreeNode *res = FindNode(key);
    if (res == nullptr) {
      throw index_out_of_bound();
    }
    return res->data_.second;
  }

  /**
   * access specified element
   * Returns a reference to the value that is mapped to a key equivalent to key,
   *   performing an insertion if such key does not already exist.
   */
  T &operator[](const Key &key) {
    RBTreeNode *res = FindNode(key);
    if (res == nullptr) {
      res = InsertNode(pair<const Key, T>(key, T())).first;
      size_++;
      if (min_node_ == nullptr || KeyCompare()(res->data_.first, min_node_->data_.first)) {
        min_node_ = res;
      }
    }
    return res->data_.second;
  }

  /**
   * behave like at() throw index_out_of_bound if such key does not exist.
   */
  const T &operator[](const Key &key) const {
    RBTreeNode *res = FindNode(key);
    if (res == nullptr) {
      throw index_out_of_bound();
    }
    return res->data_.second;
  }

  /**
   * return a iterator to the beginning
   */
  iterator begin() {
    return iterator(min_node_, this);
  }

  const_iterator cbegin() const {
    return const_iterator(min_node_, this);
  }

  /**
   * return a iterator to the end
   * in fact, it returns past-the-end.
   */
  iterator end() {
    return iterator(nullptr, this);
  }

  const_iterator cend() const {
    return const_iterator(nullptr, this);
  }

  /**
   * checks whether the container is empty
   * return true if empty, otherwise false.
   */
  bool empty() const {
    return size_ == 0;
  }

  /**
   * returns the number of elements.
   */
  size_t size() const {
    return size_;
  }

  /**
   * clears the contents
   */
  void clear() {
    ClearTree(root_);
    root_ = nullptr;
    min_node_ = nullptr;
    max_node_ = nullptr;
    size_ = 0;
  }

  /**
   * insert an element.
   * return a pair, the first of the pair is
   *   the iterator to the new element (or the element that prevented the insertion),
   *   the second one is true if insert successfully, or false.
   */
  pair<iterator, bool> insert(const value_type &value) {
    pair<RBTreeNode *, bool> res = InsertNode(value);
    if (res.second) {
      size_++;
      if (min_node_ == nullptr || KeyCompare()(value.first, min_node_->data_.first)) {
        min_node_ = res.first;
      }
      if (max_node_ == nullptr || KeyCompare()(max_node_->data_.first, value.first)) {
        max_node_ = res.first;
      }
    }
    return {iterator(res.first, this), res.second};
  }

  /**
   * erase the element at pos.
   *
   * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
   */
  void erase(iterator pos) {
    if (pos.map_ptr_ != this || pos.node_ == nullptr) {
      throw invalid_iterator();
    }
    EraseNode(pos.node_);
    if (pos.node_ == min_node_) {
      min_node_ = (root_ == nullptr ? nullptr : GetMinimum(root_));
    }
    if (pos.node_ == max_node_) {
      max_node_ = (root_ == nullptr ? nullptr : GetMaximum(root_));
    }
    size_--;
  }

  /**
   * Returns the number of elements with key
   *   that compares equivalent to the specified argument,
   *   which is either 1 or 0
   *     since this container does not allow duplicates.
   * The default method of check the equivalence is !(a < b || b > a)
   */
  size_t count(const Key &key) const {
    return FindNode(key) == nullptr ? 0 : 1;
  }

  /**
   * Finds an element with key equivalent to key.
   * key value of the element to search for.
   * Iterator to an element with key equivalent to key.
   *   If no such element is found, past-the-end (see end()) iterator is returned.
   */
  iterator find(const Key &key) {
    return iterator(FindNode(key), this);
  }

  const_iterator find(const Key &key) const {
    return const_iterator(FindNode(key), this);
  }
};

}

#endif