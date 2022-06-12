#include "compiler.hpp"
#include "runner.hpp"
#include "test_case.hpp"
#include "utils/exception.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  // contras::use_logger();
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <cdl_file> <test_file> <output_file>"
              << std::endl;
    return 1;
  }
  std::cout << "--- Contras HDL Simulator ---" << std::endl << std::endl;
  std::string cdl_file_name(argv[1]), test_file_name(argv[2]),
      output_file_name(argv[3]);
  auto compiler = std::make_shared<contras::compiler>();
  try {
    std::cout << "[1/4] Building symbol_description tree nodes" << std::endl;
    compiler->dfs_build_description_tree(cdl_file_name);
    compiler->desc_topological_sort();
    std::cout << "[2/4] Building symbol_definition map" << std::endl;
    compiler->build_definition_map();
  } catch (const contras::exception &e) {
    std::cerr << "Complication error: " << e.message() << std::endl;
    return 1;
  }
  try {
    std::cout << "[3/4] Parsing test_case input file" << std::endl;
    auto test_case =
        contras::test_case::parse_test_case(test_file_name, output_file_name);
  } catch (const contras::exception &e) {
    std::cerr << "Parsing error: " << e.message() << std::endl;
    return 1;
  }
  std::shared_ptr<contras::runner> runner;
  try {
    std::cout << "[4/4] Building runtime_instance" << std::endl;
    runner = std::make_shared<contras::runner>(
        [&]() { return compiler->build_runtime_instance(); },
        contras::test_case::parse_test_case(test_file_name, output_file_name));
  } catch (const contras::exception &e) {
    std::cerr << "Parsing error: " << e.message() << std::endl;
    return 1;
  }
  try {
    std::cout << "Finished compiling" << std::endl << std::endl;
    std::cout << "Start running..." << std::endl;
    runner->run();
  } catch (const contras::exception &e) {
    std::cerr << "Runtime error: " << e.message() << std::endl;
    return 1;
  }
  try {
    std::cout << "Writing output file..." << std::endl;
    runner->write_output_file();
  } catch (const contras::exception &e) {
    std::cerr << "Writing error: " << e.message() << std::endl;
    return 1;
  }
  std::cout << "Output file written to: " << output_file_name << std::endl;
  return 0;
}
