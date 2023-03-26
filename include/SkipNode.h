#include <utility>
template <typename T>
struct SkipNode {
    SkipNode() = default;

    SkipNode(int key_, T value)
        : key(key_), val(new T(std::move(value))) {}

    SkipNode(int key_, T *value_ptr)
        : key(key_), val(value_ptr) {}

    explicit SkipNode(SkipNode *level)
        : next_level(level) {}

    int getKey() const;
    T getVal() const;

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
    int key;
    T *val = nullptr;
};

template <typename T> inline
int SkipNode<T>::getKey() const {
    return key;
}

template <typename T> inline
T SkipNode<T>::getVal() const {
    return *val;
}

template <typename T> inline
void SkipNode<T>::release_node() {
    if (next_node == nullptr) return;
    delete next_node->val;
    next_node->val = nullptr;
}

template <typename T> inline
void SkipNode<T>::insert_node(SkipNode *node) {
    SkipNode *temp = next_node;
    next_node = node;
    node->next_node = temp;
}

template <typename T> inline
void SkipNode<T>::rm_node() {
    if (next_node == nullptr) return;
    SkipNode *temp = next_node;
    next_node = temp->next_node;
    delete temp;
}

template <typename T>
void reverse_level(SkipNode<T> *curr) {
    SkipNode<T> *prev = nullptr;
    SkipNode<T> *temp;
    while (curr) {
        temp = curr->next_level;
        curr->next_level = prev;
        prev = curr;
        curr = temp;
    }
};