#ifndef CONTRAS_SYMBOL_DESCRIPTION_HPP
#define CONTRAS_SYMBOL_DESCRIPTION_HPP

#include "pin.hpp"
#include <string>
#include <vector>

struct import_clause {
  std::string alias, file_name;
};

struct symbol_description {
  std::string file_name;
  std::vector<import_clause> import_clauses;
  // @Head
  std::string symbol_name;
  std::vector<contras::pin> input_pins, output_pins;
};

std::shared_ptr<symbol_description>
parse_symbol_description(const std::string &file_name);

// .cdl 源文件
// 不做类型检查，只做语法检查
// 1. symbol_description
// 做类型检查
// 2. symbol_definition
// 实例化
// 3. symbol_instance

#endif // CONTRAS_SYMBOL_DESCRIPTION_HPP
