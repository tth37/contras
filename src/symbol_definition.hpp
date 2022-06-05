#ifndef CONTRAS_SYMBOL_DEFINITION_HPP
#define CONTRAS_SYMBOL_DEFINITION_HPP

#include "pin.hpp"
#include "symbol_description.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace contras {

struct symbol_definition {
  std::vector<pin> input_pins, output_pins;
  std::vector<pin_with_default> state_pins;
  std::vector<logic_clause> logic_clauses;
  std::vector<pin_interface_clause> output_assign_clauses, state_assign_clauses;

  static std::shared_ptr<symbol_definition>
  parse_symbol_definition(const std::shared_ptr<symbol_description> &desc);
};

std::map<std::string, std::shared_ptr<symbol_definition>> def_map;

} // namespace contras

#endif // CONTRAS_SYMBOL_DEFINITION_HPP
