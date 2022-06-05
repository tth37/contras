#ifndef CONTRAS_SYMBOL_DESCRIPTION_HPP
#define CONTRAS_SYMBOL_DESCRIPTION_HPP

#include "pin.hpp"
#include <string>
#include <vector>

namespace contras {

struct import_clause {
  std::string alias, file_name;
};

struct pin_interface_clause {
  bool init_by_string;
  std::string name, rhs_name;
  pin_assign_clause clause;
  std::string init_str;
};

struct logic_clause {
  std::string symbol_name;
  std::vector<pin_interface_clause> input_pins, output_pins;
};

struct symbol_description {
  std::string file_name;
  std::vector<import_clause> import_clauses;
  // @Head
  std::string symbol_name;
  std::vector<pin> input_pins, output_pins;
  std::vector<pin_with_default> state_pins;
  // @Body
  std::vector<logic_clause> logic_clauses;
  // @End
  std::vector<pin_interface_clause> output_assign_clauses, state_assign_clauses;

  static std::shared_ptr<symbol_description>
  parse_symbol_description(const std::string &file_name);

  static import_clause parse_import_clause(const std::string &line);
  static std::string parse_symbol_name(const std::string &line);
  static std::vector<pin> parse_input_pin(const std::string &line);
  static std::vector<pin> parse_output_pin(const std::string &line);
  static std::vector<pin_with_default> parse_state_pin(const std::string &line);
  static pin parse_pin_def(const std::string &str);
  static pin_with_default
  parse_pin_with_default_def(const std::string &str,
                             const std::string &init_str);
  static pin_interface_clause
  parse_pin_interface_clause(const std::string &str,
                             const std::string &rhs_str);
  static std::tuple<std::string, std::size_t, std::size_t>
  parse_pin_assign_clause_one_side(const std::string &str);
  static logic_clause parse_logic_clause(const std::string &line);
  static std::vector<pin_interface_clause>
  parse_output_assign_clause(const std::string &line);
  static std::vector<pin_interface_clause>
  parse_state_assign_clause(const std::string &line);
};

} // namespace contras

#endif // CONTRAS_SYMBOL_DESCRIPTION_HPP
