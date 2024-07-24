// this value will be used because a.o precedes b.o
__attribute__((weak)) int var = 1;
// this value won't be used because there is strong variable defined in b.cpp
__attribute__((weak)) int global_var = 1;