#ifndef __SKIPLIST_H__
#define __SKIPLIST_H__

#include "SkipNode.h"
#include <random>

class SkipList {
public:
    SkipList() {
        init(max_levels);
    }

    SkipList(int level) : max_levels(level) {
        init(max_levels);
    }
    
    bool search(int target);
    
    void add(int num);
    
    bool erase(int num);

    void print();

private:
    void init(int levels);
    /*
    计算从最顶层需要向下走多少层开始插入元素
    设定非顶层元素有 1/2 的概率出现在上层
    */
    int random_levels();
    /*
    移动node，直到下一个结点的值大于等于目标值
    */
    void end_of_less(SkipNode *&node, int target);

private:
    static std::default_random_engine e;
    static std::uniform_int_distribution<int> u;
    SkipNode *rear, *head;

    size_t max_levels = 8;
};

#endif // __SKIPLIST_H__