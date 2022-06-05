#include "pin.hpp"
#include "symbol_description.hpp"
#include <iostream>

int main() {
  contras::use_logger();
  __CONTRAS_LOG(debug, "Hello, world");
  try {
    //    contras::symbol_description::parse_input_pin("INPUT aa[63:0], bb,
    //    cc");
    auto res = contras::symbol_description::parse_symbol_description("TST.cdl");
    res = res;

  } catch (const contras::exception &err) {
    std::cout << err.message() << std::endl;
  } catch (const std::exception &err) {
    std::cout << err.what() << std::endl;
  }

  system("pause");
  return 0;
}