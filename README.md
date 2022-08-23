CocoSketch
============

Repository structure
--------------------
*  `Common/`: the hash and mmap functions
*  `Struct/`: the data structures, such as heap and hash table
*  `Single/`: the single-key sketching algorithms
*  `Multiple/`: the hardware and software versions of our CocoSketch and USS

Requirements
-------
- cmake
- g++

How to run
-------
```bash
1. cmake .
2. make
3. ./CPU session-type
- session-type=1:Heavy Hitter
- session-type=2:Heavy Change
- session-type=3:1-D Hierarchical Heavy Hitter
- session-type=4:2-D Hierarchical Heavy Change
```