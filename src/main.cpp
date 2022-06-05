#include "pin.hpp"
#include <iostream>

int main() {
  contras::use_logger();
  __CONTRAS_LOG(debug, "Hello, world");
  try {
    __CONTRAS_THROW(contras::exception_type::invalid_argument, "Compile error");
  } catch (const contras::exception &err) {
    std::cout << err.message() << std::endl;
  }

  system("pause");
  return 0;
}