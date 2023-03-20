#include "SkipList.h"

int main() {
    SkipList<int> sl;
    sl.add(13, 3);
    sl.add(0,8);
    sl.add(3,8);
    sl.add(3,8);
    sl.add(9,8);
    sl.add(1,8);
    sl.add(8,8);
    sl.print(); 
    sl.erase_last(3);
    sl.print(); 
    std::cout << sl.get(13);
}