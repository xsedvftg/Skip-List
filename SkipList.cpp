﻿#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>
#include <string>

struct SkipNode {
    int val;
    SkipNode *next_level = nullptr;
    SkipNode *next_node = nullptr;

    SkipNode() = default;

    explicit SkipNode(int value)
    : val(value) {}

    explicit SkipNode(SkipNode *level)
    : next_level(level) {}

    void insert_node(SkipNode *node) {
        SkipNode *temp = next_node;
        next_node = node;
        node->next_node = temp;
    }

    void rm_node() {
        if (next_node == nullptr) return;
        SkipNode *temp = next_node;
        next_node = temp->next_node;
        delete temp;
    }

};

class SkipList {
public:
    SkipList() {
        init(max_levels);
    }

    SkipList(int level) : max_levels(level) {
        init(max_levels);
    }
    
    bool search(int target) {
        auto node = rear;

        while (node) {
            end_of_less(node, target);
                
            if (node->next_node && node->next_node->val == target) {
                return true;
            }

            node = node->next_level;
        }

        return false;
    }
    
    void add(int num) {
        SkipNode *left = rear;

        int levels = random_levels();
        end_of_less(left, num);
        while (levels--) {
            left = left->next_level;
            end_of_less(left, num);
        }

        left->insert_node(new SkipNode(num));
        SkipNode *above = left->next_node;
        while (left->next_level) {
            left = left->next_level;
            end_of_less(left, num);

            left->insert_node(new SkipNode(num));
            above->next_level = left->next_node;

            above = left->next_node;
        }
    }
    
    bool erase(int num) {
        SkipNode *node = rear;

        while (node) {
            end_of_less(node, num);

            if (node->next_node && node->next_node->val == num) {
                node->rm_node();
                while (node->next_level) {
                    node = node->next_level;
                    end_of_less(node, num);
                    node->rm_node();
                }
                return true;
            }

            node = node->next_level;
        }

        return false;
    }

    void print() {
        if (head->next_node == nullptr) return;
        auto node = head;
        /*
        统计实际有多少个元素
        以及打印一个元素需要的最大空间用于对齐打印
        记录用于打印的值
        */
        size_t num = 0;
        std::unordered_map<SkipNode *, int> uMap;
        std::vector<std::string> end_level;
        size_t max_element_len = 0;
        while (node->next_node) {
            node = node->next_node;
            uMap[node] = num;
            ++num;

            end_level.push_back(std::to_string(node->val));
            max_element_len = std::max(max_element_len, end_level.back().size());
        }

        std::vector<std::vector<std::string>> levels(max_levels, std::vector<std::string>(num));
        levels.back() = std::move(end_level);

        auto reverse_level = [](SkipNode *curr) {
            SkipNode *prev = nullptr;
            SkipNode *temp;
            while (curr) {
                temp = curr->next_level;
                curr->next_level = prev;
                prev = curr;
                curr = temp;
            }
        };

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
                auto it = uMap.find(level_node->next_level);
                size_t column = it->second;
                uMap[level_node] = column;

                (*levels_rit)[column] = levels.back()[column]; // 应更改为拷贝指针

                uMap.erase(it);
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
                if (levels[i][j].empty()) {
                    std::cout << line;
                    if (j + 1 == num || !levels[i][j + 1].empty()) {
                        std::cout << '>';
                    } else {
                        std::cout << '-';
                    }
                    next_line += empty_next_line;
                } else {
                    std::cout << levels[i][j]
                    << std::string(max_element_len - levels[i][j].size(), ' ')
                    << "-";
                    if (j + 1 == num || !levels[i][j + 1].empty()) {
                        std::cout << '>';
                    } else {
                        std::cout << '-';
                    }
                    next_line += down_pointer;
                    
                    have_element = true;
                }
            }
            if (have_element) std::cout << '\n' << next_line;
            std::cout << std::endl;
        }

    }

private:
    /*
    计算从最顶层需要向下走多少层开始插入元素
    设定非顶层元素有 1/2 的概率出现在上层
    一个颇具诱惑的做法是直接返回n - 1，似乎从上往下统计，和从下往上也差不多
    但是这样会导致一个元素插入多层为大概率事件，跳跃将变得很稀有，使性能大幅降低
    */
    int random_levels() {
        size_t n = 1;
        while (n < max_levels && u(e)) {
            ++n;
        }
        return max_levels - n;
    }
    /*
    移动node，直到下一个结点的值大于等于目标值
    */
    void end_of_less(SkipNode *&node, int target) {
        while (node->next_node && node->next_node->val < target) {
            node = node->next_node;
        }
    }

    void init(int levels) {
        SkipNode *top, *below = new SkipNode();
        head = below;
        while (--levels) {
            top = new SkipNode(below);
            below = top;
        }

        rear = below;
    }

private:
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> u;
    SkipNode *rear, *head;

    size_t max_levels = 8;
};

std::default_random_engine SkipList::e;
std::uniform_int_distribution<int> SkipList::u(0, 1);

int main() {
    SkipList sl;
    sl.add(13);
    sl.add(0);
    sl.add(3);
    // sl.erase(3);
    sl.print(); 
    std::cout << sl.erase(0);
}