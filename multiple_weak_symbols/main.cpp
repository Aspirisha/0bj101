#include <iostream>

extern int var;
extern int global_var;
extern int global_var_in_dso;

int main() {
  std::cout << "var value is: " << var << "\n";
  std::cout << "global_var value is: " << global_var << "\n";
  std::cout << "global_var_in_dso value is: " << global_var_in_dso << "\n";
  return 0;
}