#include "compiler.hpp"
#include "pin.hpp"
#include "symbol_description.hpp"
#include <iostream>

int main() {
  contras::use_logger();
  __CONTRAS_LOG(debug, "Hello, world");
  try {
    //    contras::symbol_description::parse_input_pin("INPUT aa[63:0], bb,
    //    cc");
    auto compiler = std::make_shared<contras::compiler>();
    compiler->dfs_build_description_tree("TST.cdl");
    compiler->desc_topological_sort();
    compiler->build_definition_map();
    auto inst = compiler->build_runtime_instance();
    inst->set_input_pin_from_string("a", {3, 0, 3, 0}, "1101");
    inst->execute();

    auto p = compiler;

  } catch (const contras::exception &err) {
    std::cout << err.message() << std::endl;
  } catch (const std::exception &err) {
    std::cout << err.what() << std::endl;
  }

  system("pause");
  return 0;
}