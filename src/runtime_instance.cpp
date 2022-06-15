#include "runtime_instance.hpp"

contras::runtime_instance_NAND::runtime_instance_NAND()
    : a("a", 1), b("b", 1), out("out", 1) {}

void contras::runtime_instance_NAND::clear() {
  a.clear();
  b.clear();
  out.clear();
}

void contras::runtime_instance_NAND::execute() {
  if (a.value[0] && b.value[0])
    out.assign_value_from_string("0", {0, 0, 0, 0});
  else
    out.assign_value_from_string("1", {0, 0, 0, 0});
}

void contras::runtime_instance_NAND::set_input_pin(
    const std::string &pin_name, const contras::pin_assign_clause &clause,
    const contras::pin &rhs) {
  if (pin_name == "a")
    a.assign_value(rhs, clause);
  else
    b.assign_value(rhs, clause);
}

void contras::runtime_instance_NAND::set_input_pin_from_string(
    const std::string &pin_name, const contras::pin_assign_clause &clause,
    const std::string &str) {
  if (pin_name == "a")
    a.assign_value_from_string(str, clause);
  else
    b.assign_value_from_string(str, clause);
}

contras::pin contras::runtime_instance_NAND::get_output_pin(
    const std::string &pin_name) const {
  return out;
}

contras::runtime_instance_normal::runtime_instance_normal(
    std::vector<pin> input_pins_, std::vector<pin> output_pins_,
    std::vector<pin_with_default> state_pins_,
    std::vector<symbol_instance> instances_,
    std::vector<logic_clause> logic_clauses_,
    std::vector<pin_interface_clause> output_assign_clauses_,
    std::vector<pin_interface_clause> state_assign_clauses_,
    const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map)
    : input_pins(std::move(input_pins_)), output_pins(std::move(output_pins_)),
      state_pins(std::move(state_pins_)), instances(std::move(instances_)),
      logic_clauses(std::move(logic_clauses_)),
      output_assign_clauses(std::move(output_assign_clauses_)),
      state_assign_clauses(std::move(state_assign_clauses_)) {
  for (const auto &pin : input_pins) {
    auto new_pin = pin;
    pin_map.insert(std::make_pair(new_pin.name, new_pin));
  }
  for (const auto &pin : output_pins) {
    auto new_pin = pin;
    pin_map.insert(std::make_pair(new_pin.name, new_pin));
  }
  for (const auto &pin : state_pins) {
    auto new_pin = pin;
    new_pin.initialize(pin.default_value);
    pin_map.insert(std::make_pair(new_pin.name, new_pin));
  }
  for (const auto &instance : instances) {
    const auto &def = def_map.at(instance.symbol_name);
    auto new_instance = create_runtime_instance(def, def_map);
    instance_map.insert(std::make_pair(instance.instance_name, new_instance));
  }
  for (const auto &clause : logic_clauses) {
    for (const auto &pin : clause.output_pins) {
      auto scoped_pin = contras::pin(pin.name, pin.clause.r + 1);
      scoped_pins.push_back(scoped_pin);
    }
  }
  for (const auto &pin : scoped_pins) {
    pin_map.insert(std::make_pair(pin.name, pin));
  }
}

void contras::runtime_instance_normal::clear() {
  for (const auto &pin : input_pins) {
    auto pin_name = pin.name;
    pin_map[pin_name].clear();
  }
  for (const auto &pin : output_pins) {
    auto pin_name = pin.name;
    pin_map[pin_name].clear();
  }
  for (const auto &pin : scoped_pins) {
    auto pin_name = pin.name;
    pin_map[pin_name].clear();
  }
}

void contras::runtime_instance_normal::execute() {
  for (const auto &clause : logic_clauses) {
    auto instance_name = clause.instance_name;
    auto instance = instance_map.at(instance_name);
    instance->clear();
    for (const auto &pin : clause.input_pins) {
      auto tgt_pin_name = pin.name;
      if (pin.init_by_string) {
        auto init_str = pin.init_str;
        instance->set_input_pin_from_string(tgt_pin_name, pin.clause, init_str);
      } else {
        auto src_pin_name = pin.rhs_name;
        auto src_pin = pin_map.at(src_pin_name);
        instance->set_input_pin(tgt_pin_name, pin.clause, src_pin);
      }
    }
    instance->execute();
    for (const auto &pin : clause.output_pins) {
      auto tgt_pin_name = pin.name;
      auto src_pin_name = pin.rhs_name;
      auto src_pin = instance->get_output_pin(src_pin_name);
      pin_map[tgt_pin_name].assign_value(src_pin, pin.clause);
    }
  }
  for (const auto &clause : output_assign_clauses) {
    auto tgt_pin_name = clause.name;
    auto &tgt_pin = pin_map.at(tgt_pin_name);
    if (clause.init_by_string) {
      auto init_str = clause.init_str;
      tgt_pin.assign_value_from_string(init_str, clause.clause);
    } else {
      auto src_pin_name = clause.rhs_name;
      auto src_pin = pin_map.at(src_pin_name);
      tgt_pin.assign_value(src_pin, clause.clause);
    }
  }
  for (const auto &clause : state_assign_clauses) {
    auto tgt_pin_name = clause.name;
    auto &tgt_pin = pin_map.at(tgt_pin_name);
    if (clause.init_by_string) {
      auto init_str = clause.init_str;
      tgt_pin.assign_value_from_string(init_str, clause.clause);
    } else {
      auto src_pin_name = clause.rhs_name;
      auto src_pin = pin_map.at(src_pin_name);
      tgt_pin.assign_value(src_pin, clause.clause);
    }
  }
}

void contras::runtime_instance_normal::set_input_pin(
    const std::string &pin_name, const contras::pin_assign_clause &clause,
    const contras::pin &rhs) {
  pin_map[pin_name].assign_value(rhs, clause);
}

void contras::runtime_instance_normal::set_input_pin_from_string(
    const std::string &pin_name, const contras::pin_assign_clause &clause,
    const std::string &str) {
  pin_map[pin_name].assign_value_from_string(str, clause);
}

contras::pin contras::runtime_instance_normal::get_output_pin(
    const std::string &pin_name) const {
  return pin_map.at(pin_name);
}

std::shared_ptr<contras::runtime_instance>
contras::runtime_instance::create_runtime_instance(
    const std::shared_ptr<symbol_definition> &def,
    const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map) {
  if (def->symbol_name == "NAND") {
    return std::make_shared<runtime_instance_NAND>();
  }
  return std::make_shared<runtime_instance_normal>(
      def->input_pins, def->output_pins, def->state_pins, def->instances,
      def->logic_clauses, def->output_assign_clauses, def->state_assign_clauses,
      def_map);
}
