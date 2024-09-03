#include "a.hpp"

int main() {
	func1();
  // func2();
  // func3(); // failure during link phase since func3 is declared hidden
	return 0;
}