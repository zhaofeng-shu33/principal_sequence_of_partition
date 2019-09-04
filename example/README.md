# Example program to use psp library in C++
After installation of the library on UNIX system, you can compile and run the example.
```shell
g++ -std=c++11 example.cpp -lpsp -o example
```
If `libpsp.so` is not found when you run the program, try to use the following method to resolve the dynamic link search path for current session:
```shell
export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib
```

