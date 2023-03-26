#include "SkipList.h"

int main() {
    SkipList<int> b;
    SkipList<int> sl = std::move(b);
    sl.add(13, 3);
    sl.add(34,8);
    sl.add(3,8);
    sl.add(22,8);
    sl.add(103,8);
    sl.add(1,8);
    sl.add(8,8);
    sl.print(); 
    sl.erase_last(3);
    sl.print(); 
    std::cout << sl.get(13);
}