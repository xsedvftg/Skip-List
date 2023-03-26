#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include "SkipNode.h"
#include <unordered_map>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <random>

template <typename T>
class SkipList {
public:
    SkipList() {
        init(max_levels);
    }

    explicit SkipList(int level) : max_levels(level) {
        init(max_levels);
    }
    
    SkipList(const SkipList<T> &rhs) = delete;
    SkipList<T> &operator=(const SkipList<T> &rhs) = delete;

    SkipList(SkipList<T> &&rhs) noexcept;
    SkipList<T> &operator=(SkipList<T> &&rhs) noexcept;

    bool exist(int key) const;
    
    T get(int key);

    void add(int key, T val);
    
    bool erase_last(int key);

    void print() const;

    ~SkipList();

private:
    void init(int levels);
    /*
    计算从最顶层需要向下走多少层开始插入元素
    设定非顶层元素有 1/2 的概率出现在上层
    */
    int random_levels();
    /*
    同层内移动node，直到下一个结点的值大于等于目标值
    */
    void free();

    void end_of_less(SkipNode<T> *&node, int key);

    SkipNode<T> *find_bef_first(int key);

private:
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> u;
    SkipNode<T> *rear; 
    SkipNode<T> *head; // 指向所有元素均存在的一层

    size_t max_levels = 8;
};

template <typename T>
SkipList<T>::SkipList(SkipList<T> &&rhs) noexcept
    : head(rhs.head), rear(rhs.rear), max_levels(rhs.max_levels)
{
    rhs.head = rhs.rear = nullptr;
}

template <typename T>
SkipList<T> &SkipList<T>::operator=(SkipList<T> &&rhs) noexcept {
    if (rhs == *this) return *this;

    free();
    rear = rhs.rear;
    head = rhs.head;
    max_levels = rhs.max_levels;

    rhs.head = rhs.rear = nullptr;
}

template <typename T>
std::default_random_engine SkipList<T>::e;

template <typename T>
std::uniform_int_distribution<int> SkipList<T>::u(0, 1);

template <typename T>
void SkipList<T>::init(int levels) {
    SkipNode<T> *top, *below = new SkipNode<T>();
    head = below;
    while (--levels) {
        top = new SkipNode<T>(below);
        below = top;
    }

    rear = below;
}

template <typename T>
int SkipList<T>::random_levels() {
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

template <typename T>
void SkipList<T>::end_of_less(SkipNode<T> *&node, int key) {
    while (node->next_node && node->next_node->getKey() < key) {
        node = node->next_node;
    }
}

template <typename T>
SkipNode<T> *SkipList<T>::find_bef_first(int key) {
    SkipNode<T> *node = rear;

    while (node) {
        end_of_less(node, key);
            
        if (node->next_node && node->next_node->getKey() == key) {
            return node->next_node;
        }

        node = node->next_level;
    }

    return nullptr;
}

template <typename T> inline
bool SkipList<T>::exist(int key) const {
    SkipNode<T> *node = find_bef_first(key);
    return node != nullptr;
}

template <typename T> inline
T SkipList<T>::get(int key) {
    SkipNode<T> *node = find_bef_first(key);
    if (node == nullptr) return {};
    return node->getVal();
}

template <typename T>
void SkipList<T>::add(int key, T val) {
    SkipNode<T> *left = rear;

    // 找到最上层的插入位置
    int levels = random_levels();
    end_of_less(left, key);
    while (levels--) {
        left = left->next_level;
        end_of_less(left, key);
    }

    T *value_ptr = new T(std::move(val));
    left->insert_node(new SkipNode<T>(key, value_ptr));
    SkipNode<T> *above = left->next_node;
    while (left->next_level) {
        left = left->next_level;
        end_of_less(left, key);

        left->insert_node(new SkipNode<T>(key, value_ptr));
        above->next_level = left->next_node;

        above = left->next_node;
    }
}

template <typename T>
bool SkipList<T>::erase_last(int key) {
    SkipNode<T> *node = rear;

    while (node) {
        end_of_less(node, key);

        if (node->next_node && node->next_node->getKey() == key) {
            node->release_node();
            node->rm_node();
            while (node->next_level) {
                node = node->next_level;
                end_of_less(node, key);
                node->rm_node();
            }
            return true;
        }

        node = node->next_level;
    }

    return false;
}

template <typename T>
void SkipList<T>::print() const {
    if (head->next_node == nullptr) return;
    auto node = head;
    /*
    统计实际有多少个元素
    以及打印一个元素需要的最大空间用于对齐打印
    记录用于打印的值
    */
    size_t num = 0;
    std::unordered_map<SkipNode<T> *, int> uMap;
    std::vector<std::shared_ptr<std::string>> end_level;
    size_t max_element_len = 0; // 用于统一元素占用大小对齐打印结果
    while (node->next_node) {
        node = node->next_node;
        uMap[node] = num++;

        // end_level.emplace_back(std::to_string(node->getKey()));
        end_level.push_back(std::make_shared<std::string>(std::to_string(node->getKey())));
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

            (*levels_rit)[column] = levels.back()[column]; // 应更改为拷贝指针
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

template <typename T>
void SkipList<T>::free() {
    if (!head) return; 
    SkipNode<T> *node = head;
    while (node->next_node) {
        node->release_node();
        node->rm_node();
    }

    SkipNode<T> *temp = node;
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

template <typename T>
SkipList<T>::~SkipList() {
    free();
}


#endif // __SKIPLIST_H__