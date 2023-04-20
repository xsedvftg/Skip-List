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

#define SKIPLIST_TEMPLAGE_ARGUMENTS template <typename K, typename V, typename Compare>

namespace SKIPLIST {

template <typename K, typename V, typename Compare = std::less<K>>
class SkipList {
 public:
  SkipList() { Init(max_levels); }

  explicit SkipList(int level): max_levels(level) { Init(max_levels); }

  SkipList(const SkipList &rhs) = delete;
  SkipList &operator=(const SkipList &rhs) = delete;

  SkipList(SkipList &&rhs) noexcept;
  SkipList &operator=(SkipList &&rhs) noexcept;

  bool Exist(K key) const;

  V Get(K key);

  void Add(K key, V val);

  bool Erase_last(K key);

  void Print() const;

  ~SkipList();

 private:
  /**
   * @brief 构造表的真实执行函数
   *
   * @param levels
   */
  void Init(int levels);

  /**
   * @brief 计算从最顶层需要向下走多少层开始插入元素，设定非顶层元素有 1/2 的概率出现在上层
   *
   * @return 从最顶层移动到开始插入元素的地方所需的步数
   */
  int Random_levels();

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
  void End_of_less(SkipNode<K, V> *&node, K key);

  /**
   * @brief 找到表中符合给定键的结点
   * 
   * @param key 
   * @return SkipNode<K, V>* 
   */
  SkipNode<K, V> *Find_bef_first(K key);

 private:
  static Compare compare;
  static std::default_random_engine e;
  static std::uniform_int_distribution<int> u;
  SkipNode<K, V> *rear;
  SkipNode<K, V> *head; // 指向所有元素均存在的一层

  size_t max_levels = 8;
};

SKIPLIST_TEMPLAGE_ARGUMENTS
SkipList<K, V, Compare>::SkipList(SkipList<K, V, Compare> &&rhs) noexcept
    : head(rhs.head), rear(rhs.rear), max_levels(rhs.max_levels) { rhs.head = rhs.rear = nullptr; }

SKIPLIST_TEMPLAGE_ARGUMENTS
SkipList<K, V, Compare> &SkipList<K, V, Compare>::operator=(SkipList<K, V, Compare> &&rhs) noexcept {
  if (rhs == *this) return *this;

  Free();
  rear = rhs.rear;
  head = rhs.head;
  max_levels = rhs.max_levels;

  rhs.head = rhs.rear = nullptr;
}

SKIPLIST_TEMPLAGE_ARGUMENTS
Compare SkipList<K, V, Compare>::compare;

SKIPLIST_TEMPLAGE_ARGUMENTS
std::default_random_engine SkipList<K, V, Compare>::e;

SKIPLIST_TEMPLAGE_ARGUMENTS
std::uniform_int_distribution<int> SkipList<K, V, Compare>::u(0, 1);

SKIPLIST_TEMPLAGE_ARGUMENTS
void SkipList<K, V, Compare>::Init(int levels) {
  SkipNode<K, V> *top, *below = new SkipNode<K, V>();
  head = below;
  while (--levels) {
    top = new SkipNode<K, V>(below);
    below = top;
  }

  rear = below;
}

SKIPLIST_TEMPLAGE_ARGUMENTS
int SkipList<K, V, Compare>::Random_levels() {
  size_t n = 1;
  while (n < max_levels && u(e)) {
    ++n;
  }
  /*
  一个颇具诱惑的做法是直接返回n - 1，似乎从上往下统计，和从下往上也差不多
  但是这样会导致一个元素插入多层为大概率事件，跳跃将变得很稀有，使性能大幅降低
  */
  return max_levels - n;
}

SKIPLIST_TEMPLAGE_ARGUMENTS
void SkipList<K, V, Compare>::End_of_less(SkipNode<K, V> *&node, K key) {
  while (node->next_node && compare(node->next_node->getKey(), key)) {
    node = node->next_node;
  }
}



SKIPLIST_TEMPLAGE_ARGUMENTS
SkipNode<K, V> *SkipList<K, V, Compare>::Find_bef_first(K key) {
  SkipNode<K, V> *node = rear;
  while (node) {
    End_of_less(node, key);
    if (node->next_node && node->next_node->getKey() == key) {
      return node->next_node;
    }
    node = node->next_level;
  }

  return nullptr;
}

SKIPLIST_TEMPLAGE_ARGUMENTS inline
bool SkipList<K, V, Compare>::Exist(K key) const {
  SkipNode<K, V> *node = Find_bef_first(key);
  return node != nullptr;
}

SKIPLIST_TEMPLAGE_ARGUMENTS inline
V SkipList<K, V, Compare>::Get(K key) {
  SkipNode<K, V> *node = Find_bef_first(key);
  if (node == nullptr) return {};
  return node->getVal();
}

SKIPLIST_TEMPLAGE_ARGUMENTS
void SkipList<K, V, Compare>::Add(K key, V val) {
  SkipNode<K, V> *left = rear;

  // 找到最上层的插入位置
  int levels = Random_levels();
  End_of_less(left, key);
  while (levels--) {
    left = left->next_level;
    End_of_less(left, key);
  }

  V *value_ptr = new V(std::move(val));
  left->insert_node(new SkipNode<K, V>(key, value_ptr));
  SkipNode<K, V> *above = left->next_node;
  while (left->next_level) {
    left = left->next_level;
    End_of_less(left, key);

    left->insert_node(new SkipNode<K, V>(key, value_ptr));
    above->next_level = left->next_node;

    above = left->next_node;
  }
}

SKIPLIST_TEMPLAGE_ARGUMENTS
bool SkipList<K, V, Compare>::Erase_last(K key) {
  SkipNode<K, V> *node = rear;

  while (node) {
    End_of_less(node, key);

    if (node->next_node && node->next_node->getKey() == key) {
      node->release_node();
      node->rm_node();
      while (node->next_level) {
        node = node->next_level;
        End_of_less(node, key);
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
void SkipList<K, V, Compare>::Print() const {
  if (head->next_node == nullptr) return;
  auto node = head;
  /*
  统计实际有多少个元素
  以及打印一个元素需要的最大空间用于对齐打印
  记录用于打印的值
  */
  size_t num = 0;
  std::unordered_map<SkipNode<K, V> *, int> uMap;
  std::vector<std::shared_ptr<std::string>> end_level;
  size_t max_element_len = 0; // 用于统一元素占用大小对齐打印结果
  while (node->next_node) {
    node = node->next_node;
    uMap[node] = num++;

    end_level.push_back(std::make_shared<std::string>(my_to_string(node->getKey())));
    max_element_len = std::max(max_element_len, end_level.back()->size());
  }

  std::vector<std::vector<std::shared_ptr<std::string>>>
    levels(max_levels, std::vector<std::shared_ptr<std::string>>(num));
  levels.back() = std::move(end_level);

  /*
  仅翻转虚拟结点，从底层往上遍历，方便复用上一层对应结点的列信息
  */
  reverse_level(rear);
  auto levels_rit = levels.rbegin();
  node = head;
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
  reverse_level(head);

  /*
  真实打印逻辑
  */
  std::string line(max_element_len + 1, '-');
  std::string down_pointer("↓"); down_pointer.insert(down_pointer.end(), max_element_len + 1, ' ');
  std::string empty_next_line(max_element_len + 2, ' ');
  for (size_t i = 0; i < max_levels; ++i) {
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
void SkipList<K, V, Compare>::Free() {
  if (!head) return;
  SkipNode<K, V> *node = head;
  while (node->next_node) {
    node->release_node();
    node->rm_node();
  }

  SkipNode<K, V> *temp = node;
  node = rear;
  delete temp;

  while (node != head) {
    while (node->next_node) {
      node->rm_node();
    }
    temp = node;
    node = node->next_level;
    delete temp;
  }
}

SKIPLIST_TEMPLAGE_ARGUMENTS SkipList<K, V, Compare>::~SkipList() { Free(); }

}// namespace SKIPLIST

#endif // __SKIPLIST_H__