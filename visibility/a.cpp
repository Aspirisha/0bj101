#include "a.hpp"

#include <iostream>


void func1() {
  std::cout << "func1()\n";
}


static void func2() {
  std::cout << "func2()\n";
}

void func3() {
  std::cout << "func3()\n";
} 