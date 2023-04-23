#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include "SkipNode.h"
#include <unordered_map>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <random>
#include <functional>

#define SKIPLIST_TEMPLAGE_ARGUMENTS template <typename K, typename V, typename CompareType>
#define SKIPLIST_TYPE SkipList<K, V, CompareType>

namespace SKIPLIST {

template <typename K, typename V, typename CompareType = std::less<K>>
class SkipList {
 public:
  SkipList() { Init(max_levels_); }

  explicit SkipList(size_t level): max_levels_(level) { Init(max_levels_); }

  SkipList(const SkipList &rhs) = delete;
  SkipList &operator=(const SkipList &rhs) = delete;

  SkipList(SkipList &&rhs) noexcept;
  SkipList &operator=(SkipList &&rhs) noexcept;

  bool Exist(const K &key) const;

  const V &Get(const K &key);

  void Add(const K &key, const V &val);

  bool Erase_last(const K &key);

  void Print() const;

  ~SkipList();

  template <typename K_, typename V_>
  class Iterator {
   public:
    Iterator(SkipNode<K_, V_> *iter) : node_(iter) {}
    Iterator operator++() {
      if (node_) {
        node_ = node_->next_node;
      }
      return *this;
    }
    bool operator==(const Iterator &iter) const { return iter.node_ == node_; }
    bool operator!=(const Iterator &iter) const { return !(*this == iter); }
    operator bool() const { return node_ != nullptr; }
    SkipNode<K_, V_> *operator*() const {
      if (node_ == nullptr) {
        throw std::runtime_error("dereference null iterator");
      }
      return node_;
    }
   private:
    SkipNode<K_, V_> *node_;
  };

  Iterator<K, V> begin() { return Iterator<K, V>(head_->next_node); }
  Iterator<K, V> end() { return Iterator<K, V>(nullptr); }
  Iterator<K, V> GetIter(const K &key) {
    auto node = FindNode(key);
    while (node && node->next_level) node = node->next_level;
    return Iterator<K, V>(node);
  }
  
 private:
  /**
   * @brief 构造表的真实执行函数
   *
   * @param levels
   */
  void Init(size_t levels);

  /**
   * @brief 计算从最顶层需要向下走多少层开始插入元素，设定非顶层元素有 1/2 的概率出现在上层
   *
   * @return 从最顶层移动到开始插入元素的地方所需的步数
   */
  int RandomLevels();

  /**
   * @brief 析构表的真实执行函数
   *
   */
  void Free();

  /**
   * @brief 同层内移动node，直到下一个结点的值大于等于目标值
   * 
   * @param node 需要移动的结点
   * @param key 结点最终所处位置的上界
   */
  void EndOfLess(SKIPNODE_TYPE *&node, const K &key);

  /**
   * @brief 找到表中符合给定键的结点，不一定是底层结点
   * 
   * @param key 
   * @return SKIPNODE_TYPE* 
   */
  SKIPNODE_TYPE *FindNode(const K &key);

 private:
  static CompareType compare_;
  static std::default_random_engine e_;
  static std::uniform_int_distribution<int> u_;
  SKIPNODE_TYPE *rear_;
  SKIPNODE_TYPE *head_; // 指向所有元素均存在的一层

  size_t max_levels_ = 8;
};

SKIPLIST_TEMPLAGE_ARGUMENTS
SKIPLIST_TYPE::SkipList(SkipList<K, V, CompareType> &&rhs) noexcept
    : head_(rhs.head_), rear_(rhs.rear_), max_levels_(rhs.max_levels_) { rhs.head_ = rhs.rear_ = nullptr; }

SKIPLIST_TEMPLAGE_ARGUMENTS
SKIPLIST_TYPE &SKIPLIST_TYPE::operator=(SkipList<K, V, CompareType> &&rhs) noexcept {
  if (rhs == *this) return *this;

  Free();
  rear_ = rhs.rear_;
  head_ = rhs.head_;
  max_levels_ = rhs.max_levels_;

  rhs.head_ = rhs.rear_ = nullptr;
}

SKIPLIST_TEMPLAGE_ARGUMENTS
CompareType SKIPLIST_TYPE::compare_;

SKIPLIST_TEMPLAGE_ARGUMENTS
std::default_random_engine SKIPLIST_TYPE::e_;

SKIPLIST_TEMPLAGE_ARGUMENTS
std::uniform_int_distribution<int> SKIPLIST_TYPE::u_(0, 1);

SKIPLIST_TEMPLAGE_ARGUMENTS
void SKIPLIST_TYPE::Init(size_t levels) {
  SKIPNODE_TYPE *top, *below = new SKIPNODE_TYPE();
  head_ = below;
  while (--levels) {
    top = new SKIPNODE_TYPE(below);
    below = top;
  }

  rear_ = below;
}

SKIPLIST_TEMPLAGE_ARGUMENTS
int SKIPLIST_TYPE::RandomLevels() {
  size_t n = 1;
  while (n < max_levels_ && u_(e_)) {
    ++n;
  }
  /*
  一个颇具诱惑的做法是直接返回n - 1，似乎从上往下统计，和从下往上也差不多
  但是这样会导致一个元素插入多层为大概率事件，跳跃将变得很稀有，使性能大幅降低
  */
  return max_levels_ - n;
}

SKIPLIST_TEMPLAGE_ARGUMENTS
void SKIPLIST_TYPE::EndOfLess(SKIPNODE_TYPE *&node, const K &key) {
  while (node->next_node && compare_(node->next_node->getKey(), key)) {
    node = node->next_node;
  }
}

SKIPLIST_TEMPLAGE_ARGUMENTS
SKIPNODE_TYPE *SKIPLIST_TYPE::FindNode(const K &key) {
  SKIPNODE_TYPE *node = rear_;
  while (node) {
    EndOfLess(node, key);
    if (node->next_node && node->next_node->getKey() == key) {
      return node->next_node;
    }
    node = node->next_level;
  }

  return nullptr;
}

SKIPLIST_TEMPLAGE_ARGUMENTS inline
bool SKIPLIST_TYPE::Exist(const K &key) const {
  SKIPNODE_TYPE *node = FindNode(key);
  return node != nullptr;
}

SKIPLIST_TEMPLAGE_ARGUMENTS inline
const V &SKIPLIST_TYPE::Get(const K &key) {
  SKIPNODE_TYPE *node = FindNode(key);
  if (node == nullptr) {
    throw std::runtime_error("no such key");
  }
  return node->getVal();
}

SKIPLIST_TEMPLAGE_ARGUMENTS
void SKIPLIST_TYPE::Add(const K &key, const V &val) {
  SKIPNODE_TYPE *left = rear_;

  // 找到最上层的插入位置
  int levels = RandomLevels();
  EndOfLess(left, key);
  while (levels--) {
    left = left->next_level;
    EndOfLess(left, key);
  }

  V *value_ptr = new V(std::move(val));
  left->insert_node(new SKIPNODE_TYPE(key, value_ptr));
  SKIPNODE_TYPE *above = left->next_node;
  while (left->next_level) {
    left = left->next_level;
    EndOfLess(left, key);

    left->insert_node(new SKIPNODE_TYPE(key, value_ptr));
    above->next_level = left->next_node;

    above = left->next_node;
  }
}

SKIPLIST_TEMPLAGE_ARGUMENTS
bool SKIPLIST_TYPE::Erase_last(const K &key) {
  SKIPNODE_TYPE *node = rear_;

  while (node) {
    EndOfLess(node, key);

    if (node->next_node && node->next_node->getKey() == key) {
      node->release_node();
      node->rm_node();
      while (node->next_level) {
        node = node->next_level;
        EndOfLess(node, key);
        node->rm_node();
      }
      return true;
    }

    node = node->next_level;
  }

  return false;
}

template <typename K> std::string my_to_string(const K &key) { return std::to_string(key); }

template <> std::string my_to_string(const std::string &s) { return s; }

SKIPLIST_TEMPLAGE_ARGUMENTS
void SKIPLIST_TYPE::Print() const {
  if (head_->next_node == nullptr) return;
  auto node = head_;
  /*
  统计实际有多少个元素
  以及打印一个元素需要的最大空间用于对齐打印
  记录用于打印的值
  */
  size_t num = 0;
  std::unordered_map<SKIPNODE_TYPE *, int> uMap;
  std::vector<std::shared_ptr<std::string>> end_level;
  size_t max_element_len = 0; // 用于统一元素占用大小对齐打印结果
  while (node->next_node) {
    node = node->next_node;
    uMap[node] = num++;

    end_level.push_back(std::make_shared<std::string>(my_to_string(node->getKey())));
    max_element_len = std::max(max_element_len, end_level.back()->size());
  }

  std::vector<std::vector<std::shared_ptr<std::string>>>
    levels(max_levels_, std::vector<std::shared_ptr<std::string>>(num));
  levels.back() = std::move(end_level);

  /*
  仅翻转虚拟结点，从底层往上遍历，方便复用上一层对应结点的列信息
  */
  reverse_level(rear_);
  auto levels_rit = levels.rbegin();
  node = head_;
  while (node->next_level) {
    node = node->next_level;
    ++levels_rit;

    auto level_node = node;
    while (level_node->next_node) {
      level_node = level_node->next_node;
      size_t column = uMap.find(level_node->next_level)->second;
      uMap[level_node] = column;

      (*levels_rit)[column] = levels.back()[column];
    }
  }
  reverse_level(head_);

  /*
  真实打印逻辑
  */
  std::string line(max_element_len + 1, '-');
  std::string down_pointer("↓"); down_pointer.insert(down_pointer.end(), max_element_len + 1, ' ');
  std::string empty_next_line(max_element_len + 2, ' ');
  for (size_t i = 0; i < max_levels_; ++i) {
    std::string next_line;
    bool have_element = false;
    for (size_t j = 0; j < num; ++j) {
      if (levels[i][j] == nullptr) {
        // 打印 --
        std::cout << line;
        if (j + 1 == num || levels[i][j + 1]) {
          std::cout << '>';
        } else {
          std::cout << '-';
        }
        next_line += empty_next_line;
      } else {
        std::cout << *levels[i][j]
          << std::string(max_element_len - levels[i][j]->size(), ' ')
          << "-";
        if (j + 1 == num || levels[i][j + 1]) {
          std::cout << '>';
        } else {
          std::cout << '-';
        }
        next_line += down_pointer;

        have_element = true;
      }
    }
    if (have_element) std::cout << '\n' << next_line;
    std::cout << '\n';
  }
  std::cout << std::endl;

}

SKIPLIST_TEMPLAGE_ARGUMENTS
void SKIPLIST_TYPE::Free() {
  if (!head_) return;
  SKIPNODE_TYPE *node = head_;
  while (node->next_node) {
    node->release_node();
    node->rm_node();
  }

  SKIPNODE_TYPE *temp = node;
  node = rear_;
  delete temp;

  while (node != head_) {
    while (node->next_node) {
      node->rm_node();
    }
    temp = node;
    node = node->next_level;
    delete temp;
  }
}

SKIPLIST_TEMPLAGE_ARGUMENTS SKIPLIST_TYPE::~SkipList() { Free(); }

}// namespace SKIPLIST

#endif // __SKIPLIST_H__