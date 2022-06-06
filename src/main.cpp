#include "compiler.hpp"
#include "pin.hpp"
#include "runner.hpp"
#include "symbol_description.hpp"
#include "test_case.hpp"
#include <iostream>

int main() {
  contras::use_logger();
  __CONTRAS_LOG(debug, "Hello, world");
  try {
    //    contras::symbol_description::parse_input_pin("INPUT aa[63:0], bb,
    //    cc");
    auto compiler = std::make_shared<contras::compiler>();
    compiler->dfs_build_description_tree("TTL.cdl");
    compiler->desc_topological_sort();
    compiler->build_definition_map();

    auto single_test_case = contras::single_test_case();
    single_test_case.length = 4;
    single_test_case.input_sequences = {{"a", 0, {1, 1, 1, 1}}};
    single_test_case.output_sequences = {{"out", 0, {0, 0, 0, 0}}};
    auto test_case = std::make_shared<contras::test_case>();
    test_case->test_cases.push_back(single_test_case);

    auto runner = std::make_shared<contras::runner>(
        [&]() { return compiler->build_runtime_instance(); }, test_case);

    runner->run();

  } catch (const contras::exception &err) {
    std::cout << err.message() << std::endl;
  } catch (const std::exception &err) {
    std::cout << err.what() << std::endl;
  }

  system("pause");
  return 0;
}