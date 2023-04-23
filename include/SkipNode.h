#include <utility>

#define SKIPNODE_TEMPLATE_ARGUMENTS template <typename K, typename V>
#define SKIPNODE_TYPE SkipNode<K, V>

namespace SKIPLIST {

SKIPNODE_TEMPLATE_ARGUMENTS
struct SkipNode {
  SkipNode() = default;

  SkipNode(const K &key_, const V &value) : key(key_), val(new V(std::move(value))) {}

  SkipNode(const K &key_, V *value_ptr) : key(key_), val(value_ptr) {}

  explicit SkipNode(SkipNode *level) : next_level(level) {}

  const K &getKey() const;
  const V &getVal() const;

  /**
   * @brief 析构同层后方存储的值，因为同列结点设计为指向同一个值对象，只需释放一次即可；析构函数中释放内存并设置空指针，多次析构也会有额外开销，
   * shared_ptr的引用计数也不例外，故提供手动释放内存的接口
   *
   */
  void release_node();

  /**
   * @brief 在同层后方插入结点
   *
   * @param node 需要插入结点的指针
   */
  void insert_node(SkipNode *node);

  /**
   * @brief 移除同层的下一个结点
   * 
   */
  void rm_node();

  SkipNode *next_level = nullptr;
  SkipNode *next_node = nullptr; // 同层后方结点
 private:
  K key;
  V *val = nullptr;
};

SKIPNODE_TEMPLATE_ARGUMENTS inline
const K &SKIPNODE_TYPE::getKey() const {
  return key;
}

SKIPNODE_TEMPLATE_ARGUMENTS inline
const V &SKIPNODE_TYPE::getVal() const {
  return *val;
}

SKIPNODE_TEMPLATE_ARGUMENTS inline
void SKIPNODE_TYPE::release_node() {
  if (next_node == nullptr) return;
  delete next_node->val;
  next_node->val = nullptr;
}

SKIPNODE_TEMPLATE_ARGUMENTS inline
void SKIPNODE_TYPE::insert_node(SkipNode *node) {
  SkipNode *temp = next_node;
  next_node = node;
  node->next_node = temp;
}

SKIPNODE_TEMPLATE_ARGUMENTS inline
void SKIPNODE_TYPE::rm_node() {
  if (next_node == nullptr) return;
  SkipNode *temp = next_node;
  next_node = temp->next_node;
  delete temp;
}

SKIPNODE_TEMPLATE_ARGUMENTS
void reverse_level(SkipNode<K, V> *curr) {
  SkipNode<K, V> *prev = nullptr;
  SkipNode<K, V> *temp;
  while (curr) {
    temp = curr->next_level;
    curr->next_level = prev;
    prev = curr;
    curr = temp;
  }
};

}// namespace SKIPLIST