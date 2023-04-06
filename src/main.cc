#include "SkipList.h"

int main() {
  // 示例用法
  SkipList<int, int> origin_int;
  SkipList<int, int> used = std::move(origin_int);
  used.add(13, 3);
  used.add(34, 8);
  used.add(3, 8);
  used.add(22, 8);
  used.add(103, 8);
  used.add(1, 8);
  used.add(8, 8);
  used.print();
  used.erase_last(3);
  used.print();
  std::cout << used.get(13) << '\n';

  // 示例用法
  SkipList<std::string, int> origin_string;
  SkipList<std::string, int> used2 = std::move(origin_string);
  used2.add("13", 3);
  used2.add("34", 8);
  used2.add("3", 8);
  used2.add("22", 8);
  used2.add("103", 8);
  used2.add("1", 8);
  used2.add("8", 8);
  used2.print();
  used2.erase_last("3");
  used2.print();
  std::cout << used2.get("13") << std::endl;

}