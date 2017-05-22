# C++ ESW server

Uses:
- libmicrohttpd
- zlib
- C++0x standard

How to build:

```sh
g++ -static -pthread \
dictionary.cpp server.cpp main.cpp \
-std=c++0x -lmicrohttpd -O3 -ffast-math \
-lz -lrt \
-I /path/to/zlib/include \
-I /path/to/libmicrohttpd/include \
-L /path/to/zlib/lib/ \
-L /path/to/libmicrohttpd/lib \
-o server
```

Then run with:

```sh
./server
```

