#include <utility>
template <typename K, typename V>
struct SkipNode {
  SkipNode() = default;

  SkipNode(K key_, V value)
    : key(key_), val(new V(std::move(value))) {}

  SkipNode(K key_, V *value_ptr)
    : key(key_), val(value_ptr) {}

  explicit SkipNode(SkipNode *level)
    : next_level(level) {}

  const K &getKey() const;
  const V &getVal() const;

  /*
  析构同层后方存储的值
  因为同列结点设计为指向同一个值对象，只需释放一次即可
  析构函数中释放内存并设置空指针，多次析构也会有额外开销，shared_ptr的引用计数也不例外
  故提供手动释放内存的接口
  */
  void release_node();

  /*
  在同层后方插入结点
  */
  void insert_node(SkipNode *node);

  /*
  移除同层的下一个结点
  */
  void rm_node();

  SkipNode *next_level = nullptr;
  SkipNode *next_node = nullptr; // 同层后方结点
private:
  K key;
  V *val = nullptr;
};

template <typename K, typename V> inline
const K &SkipNode<K, V>::getKey() const {
  return key;
}

template <typename K, typename V> inline
const V &SkipNode<K, V>::getVal() const {
  return *val;
}

template <typename K, typename V> inline
void SkipNode<K, V>::release_node() {
  if (next_node == nullptr) return;
  delete next_node->val;
  next_node->val = nullptr;
}

template <typename K, typename V> inline
void SkipNode<K, V>::insert_node(SkipNode *node) {
  SkipNode *temp = next_node;
  next_node = node;
  node->next_node = temp;
}

template <typename K, typename V> inline
void SkipNode<K, V>::rm_node() {
  if (next_node == nullptr) return;
  SkipNode *temp = next_node;
  next_node = temp->next_node;
  delete temp;
}

template <typename K, typename V>
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