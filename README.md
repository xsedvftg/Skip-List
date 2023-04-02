# A Tiny key-value storage library based on skip list

Intuitive implementation

## Features

* Data is stored sorted by key, key can be of any type.

* You can specify any sort method; by default, std::less is used

* The basic operations are `add(key, val)`, `erase_last(key)`, `get(key)`, `print()`

* using `skiplist.print()` to output the structure graph through standard output

  * example:

    ```
    ---------------------------------->
    ---------------------------------->
    ---------------------------------->
    ---------------------------------->
    1  ------------------------------->
    ↓
    1  ------>8  --------------------->
    ↓         ↓
    1  ->3  ->8  ------>22 ----------->
    ↓    ↓    ↓         ↓
    1  ->3  ->8  ->13 ->22 ->34 ->103->
    ↓    ↓    ↓    ↓    ↓    ↓    ↓
    ```
    
    

## Limitations

* The value type have to be destructible with delete keyword.
* It can store multiple values with the same key, but with `erase_last(key)` only the last inserted element with the same key can be removed each time.

* There is no C++ standard specified in `CMakeLists.txt`, but `C++11` should be the minimum.

## Reference

[Skip Lists: A Probabilistic Alternative to Balanced Trees](https://leetcode.cn/link/?target=https://15721.courses.cs.cmu.edu/spring2018/papers/08-oltpindexes1/pugh-skiplists-cacm1990.pdf)

[Skip list - Wikipedia](https://en.wikipedia.org/wiki/Skip_list)