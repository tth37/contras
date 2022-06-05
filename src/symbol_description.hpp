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

#endif // CONTRAS_SYMBOL_DESCRIPTION_HPP
