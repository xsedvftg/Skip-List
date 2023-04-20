#include "SkipList.h"

int main() {
  // 示例用法
  SKIPLIST::SkipList<int, int> origin_int;
  SKIPLIST::SkipList<int, int> used = std::move(origin_int);
  used.Add(13, 3);
  used.Add(34, 8);
  used.Add(3, 8);
  used.Add(22, 8);
  used.Add(103, 8);
  used.Add(1, 8);
  used.Add(8, 8);
  used.Print();
  used.Erase_last(3);
  used.Print();
  std::cout << used.Get(13) << '\n';

  // 示例用法
  SKIPLIST::SkipList<std::string, int> origin_string;
  SKIPLIST::SkipList<std::string, int> used2 = std::move(origin_string);
  used2.Add("13", 3);
  used2.Add("34", 8);
  used2.Add("3", 8);
  used2.Add("22", 8);
  used2.Add("103", 8);
  used2.Add("1", 8);
  used2.Add("8", 8);
  used2.Print();
  used2.Erase_last("3");
  used2.Print();
  std::cout << used2.Get("13") << std::endl;
}