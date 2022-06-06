#ifndef CONTRAS_SYMBOL_DEFINITION_HPP
#define CONTRAS_SYMBOL_DEFINITION_HPP

#include "pin.hpp"
#include "symbol_description.hpp"
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace contras {

struct pin_with_meta {
  pin pin{"", 0};
  std::string meta;
};

struct symbol_definition {
  std::string symbol_name;
  std::vector<pin> input_pins, output_pins;
  std::vector<pin_with_default> state_pins;
  std::vector<symbol_instance> instances;
  std::vector<logic_clause> logic_clauses;
  std::vector<pin_interface_clause> output_assign_clauses, state_assign_clauses;

  static std::shared_ptr<symbol_definition> parse_symbol_definition(
      const std::shared_ptr<symbol_description> &desc,
      const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map);
  static std::shared_ptr<symbol_definition> parse_NAND_definition();
  static bool start_with_digit(const std::string &str);
  static void
  check_variable_names(const std::shared_ptr<symbol_description> &desc);
  static void check_variable_types(
      const std::shared_ptr<symbol_description> &desc,
      const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map);
  static pin
  get_input_pin_from_def(const std::string &pin_name,
                         const std::shared_ptr<symbol_definition> &def);
  static pin
  get_output_pin_from_def(const std::string &pin_name,
                          const std::shared_ptr<symbol_definition> &def);
  static pin
  get_src_pin_from_map(const std::string &pin_name,
                       const std::map<std::string, pin_with_meta> &pin_map);
};

} // namespace contras

#endif // CONTRAS_SYMBOL_DEFINITION_HPP
