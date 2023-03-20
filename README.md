# A Tiny key-value storage library based on skip list

simple and crude

## Features

* Data is stored sorted by key.

* The basic operations are `add(key, val)`, `erase_last(key)`, `get(key)`

* using `obj.print()` to show the structure in your CLI

  * example:

    ![image-20230320120841007](.\example.png)

## Limitations

* Only int is supported as the key type.
* The value type have to be destructible with delete keyword
* It can store multiple values with the same key, but with `erase_last(key)` only the last inserted element with the same key can be removed each time

* There is no c++ standard specified in CMakeLists.txt, but c++11 should be the minimum

## Reference

[Skip Lists: A Probabilistic Alternative to Balanced Trees](https://leetcode.cn/link/?target=https://15721.courses.cs.cmu.edu/spring2018/papers/08-oltpindexes1/pugh-skiplists-cacm1990.pdf)

[Skip list - Wikipedia](https://en.wikipedia.org/wiki/Skip_list)