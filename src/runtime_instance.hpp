#ifndef CONTRAS_RUNTIME_INSTANCE_HPP
#define CONTRAS_RUNTIME_INSTANCE_HPP

#include "pin.hpp"
#include "symbol_definition.hpp"
#include "symbol_description.hpp"
#include <map>
#include <vector>

namespace contras {

class runtime_instance {
public:
  runtime_instance() = default;
  virtual void clear() = 0;
  virtual void execute() = 0;
  virtual void set_input_pin(const std::string &pin_name,
                             const pin_assign_clause &clause,
                             const pin &rhs) = 0;
  virtual void set_input_pin_from_string(const std::string &pin_name,
                                         const pin_assign_clause &clause,
                                         const std::string &str) = 0;
  [[nodiscard]] virtual pin
  get_output_pin(const std::string &pin_name) const = 0;

  static std::shared_ptr<runtime_instance> create_runtime_instance(
      const std::shared_ptr<symbol_definition> &def,
      const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map);
};

class runtime_instance_NAND : public runtime_instance {
public:
  runtime_instance_NAND();
  void clear() override;
  void execute() override;
  void set_input_pin(const std::string &pin_name,
                     const pin_assign_clause &clause, const pin &rhs) override;
  void set_input_pin_from_string(const std::string &pin_name,
                                 const pin_assign_clause &clause,
                                 const std::string &str) override;
  [[nodiscard]] pin get_output_pin(const std::string &pin_name) const override;

private:
  pin a, b, out;
};

class runtime_instance_normal : public runtime_instance {
public:
  runtime_instance_normal(
      std::vector<pin> input_pins_, std::vector<pin> output_pins_,
      std::vector<pin_with_default> state_pins_,
      std::vector<symbol_instance> instances_,
      std::vector<logic_clause> logic_clauses_,
      std::vector<pin_interface_clause> output_assign_clauses_,
      std::vector<pin_interface_clause> state_assign_clauses_,
      const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map);
  void clear() override;
  void execute() override;
  void set_input_pin(const std::string &pin_name,
                     const pin_assign_clause &clause, const pin &rhs) override;
  void set_input_pin_from_string(const std::string &pin_name,
                                 const pin_assign_clause &clause,
                                 const std::string &str) override;
  [[nodiscard]] pin get_output_pin(const std::string &pin_name) const override;

private:
  std::vector<pin> input_pins, output_pins, scoped_pins;
  std::vector<pin_with_default> state_pins;
  std::vector<symbol_instance> instances;
  std::vector<logic_clause> logic_clauses;
  std::vector<pin_interface_clause> output_assign_clauses, state_assign_clauses;
  std::map<std::string, pin> pin_map;
  std::map<std::string, std::shared_ptr<runtime_instance>> instance_map;
};

} // namespace contras

#endif // CONTRAS_RUNTIME_INSTANCE_HPP
