#include "symbol_definition.hpp"

bool contras::symbol_definition::start_with_digit(const std::string &str) {
  return std::isdigit(str[0]);
}

std::shared_ptr<contras::symbol_definition>
contras::symbol_definition::parse_symbol_definition(
    const std::shared_ptr<symbol_description> &desc,
    const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map) {
  check_variable_names(desc);
  check_variable_types(desc, def_map);
  auto res = std::make_shared<symbol_definition>();
  res->symbol_name = desc->symbol_name;
  for (auto &pin : desc->input_pins) {
    res->input_pins.push_back(pin);
  }
  for (auto &pin : desc->output_pins) {
    res->output_pins.push_back(pin);
  }
  for (auto &pin : desc->state_pins) {
    res->state_pins.push_back(pin);
  }
  for (auto &inst : desc->instances) {
    res->instances.push_back(inst);
  }
  for (auto &clause : desc->logic_clauses) {
    res->logic_clauses.push_back(clause);
  }
  for (auto &clause : desc->output_assign_clauses) {
    res->output_assign_clauses.push_back(clause);
  }
  for (auto &clause : desc->state_assign_clauses) {
    res->state_assign_clauses.push_back(clause);
  }
  return res;
}

void contras::symbol_definition::check_variable_names(
    const std::shared_ptr<symbol_description> &desc) {
  if (start_with_digit(desc->symbol_name)) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "symbol_name cannot start with digit");
  }
  auto import_clauses = desc->import_clauses;
  for (const auto &import_clause : import_clauses) {
    if (start_with_digit(import_clause.alias)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "alias cannot start with digit");
    }
  }
  auto input_pins = desc->input_pins;
  for (const auto &pin : input_pins) {
    if (start_with_digit(pin.name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "input_pin name cannot start with digit");
    }
  }
  auto output_pins = desc->output_pins;
  for (const auto &pin : output_pins) {
    if (start_with_digit(pin.name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "output_pin name cannot start with digit");
    }
  }
  auto state_pins = desc->state_pins;
  for (const auto &pin : state_pins) {
    if (start_with_digit(pin.name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "state_pin name cannot start with digit");
    }
  }
  auto instances = desc->instances;
  for (const auto &instance : instances) {
    if (start_with_digit(instance.instance_name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "instance name cannot start with digit");
    }
    if (start_with_digit(instance.symbol_name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "instance symbol_name cannot start with digit");
    }
  }
  std::set<std::string> instance_name_set;
  auto logic_clauses = desc->logic_clauses;
  for (const auto &clause : logic_clauses) {
    auto instance_name = clause.instance_name;
    if (instance_name_set.find(instance_name) != instance_name_set.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "reuse of instance name");
    }
    auto clause_input_pins = clause.input_pins;
    for (const auto &pin : clause_input_pins) {
      if (start_with_digit(pin.name)) {
        __CONTRAS_THROW(exception_type::invalid_argument,
                        "logic_clause input_pin name cannot start with digit");
      }
      if (!pin.init_by_string && start_with_digit(pin.rhs_name)) {
        __CONTRAS_THROW(
            exception_type::invalid_argument,
            "logic_clause input_pin rhs_name cannot start with digit");
      }
    }
    auto clause_output_pins = clause.output_pins;
    for (const auto &pin : clause_output_pins) {
      if (start_with_digit(pin.name)) {
        __CONTRAS_THROW(exception_type::invalid_argument,
                        "logic_clause output_pin name cannot start with digit");
      }
      if (pin.init_by_string) {
        __CONTRAS_THROW(
            exception_type::invalid_argument,
            "logic_clause output_pin cannot be initialized by string");
      }
    }
  }
  auto output_assign_clauses = desc->output_assign_clauses;
  for (const auto &pin : output_assign_clauses) {
    if (start_with_digit(pin.name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "output_assign_clause name cannot start with digit");
    }
    if (!pin.init_by_string && start_with_digit(pin.rhs_name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "output_assign_clause rhs_name cannot start with digit");
    }
  }
  auto state_assign_clauses = desc->state_assign_clauses;
  for (const auto &pin : state_assign_clauses) {
    if (start_with_digit(pin.name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "state_assign_clause name cannot start with digit");
    }
    if (!pin.init_by_string && start_with_digit(pin.rhs_name)) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "state_assign_clause rhs_name cannot start with digit");
    }
  }
}

void contras::symbol_definition::check_variable_types(
    const std::shared_ptr<symbol_description> &desc,
    const std::map<std::string, std::shared_ptr<symbol_definition>> &def_map) {
  std::map<std::string, std::shared_ptr<symbol_definition>> alias_map;
  for (const auto &import_clause : desc->import_clauses) {
    auto def = def_map.find(import_clause.alias);
    if (def == def_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "import_clause symbol_name not found");
    }
    if (alias_map.find(import_clause.alias) != alias_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "import_clause alias already exists");
    }
    alias_map[import_clause.alias] = def_map.at(import_clause.alias);
  }

  std::map<std::string, std::shared_ptr<symbol_definition>> instance_map;
  for (const auto &instance : desc->instances) {
    auto def = alias_map.find(instance.symbol_name);
    if (def == alias_map.end() && instance.symbol_name != "NAND") {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "instance symbol_name not found");
    }
    if (instance_map.find(instance.instance_name) != instance_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "instance instance_name already exists");
    }
    if (instance.symbol_name == "NAND")
      instance_map[instance.instance_name] = def_map.at("NAND");
    else
      instance_map[instance.instance_name] = alias_map[instance.symbol_name];
  }

  std::map<std::string, pin_with_meta> pin_map;
  for (const auto &pin : desc->input_pins) {
    auto pin_name = pin.name;
    if (pin_map.find(pin_name) != pin_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "input_pin name already exists");
    }
    pin_map[pin_name] = {pin, "input"};
  }
  for (const auto &pin : desc->output_pins) {
    auto pin_name = pin.name;
    if (pin_map.find(pin_name) != pin_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "output_pin name already exists");
    }
    pin_map[pin_name] = {pin, "output"};
  }
  for (const auto &pin : desc->state_pins) {
    auto pin_name = pin.name;
    if (pin_map.find(pin_name) != pin_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "state_pin name already exists");
    }
    auto raw_pin = contras::pin{pin.name, pin.length};
    pin_map[pin_name] = {raw_pin, "state"};
  }

  for (const auto &logic_clause : desc->logic_clauses) {
    auto instance_name = logic_clause.instance_name;
    if (instance_map.find(instance_name) == instance_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "logic_clause instance_name not found");
    }
    auto def = instance_map[instance_name];
    for (const auto &pin : logic_clause.input_pins) {
      auto tgt_pin_name = pin.name;
      auto tgt_pin = get_input_pin_from_def(tgt_pin_name, def);
      if (pin.init_by_string) {
        auto init_str = pin.init_str;
        tgt_pin.validate_assign_from_string(init_str, pin.clause);
      } else {
        auto src_pin_name = pin.rhs_name;
        auto src_pin = get_src_pin_from_map(src_pin_name, pin_map);
        tgt_pin.validate_assign_clause(src_pin, pin.clause);
      }
    }
    for (const auto &pin : logic_clause.output_pins) {
      auto tgt_pin_name = pin.name;
      if (pin_map.find(tgt_pin_name) != pin_map.end()) {
        __CONTRAS_THROW(exception_type::invalid_argument,
                        "logic_clause output_pin name already exists");
      }
      auto tgt_pin = contras::pin{tgt_pin_name, pin.clause.r + 1};
      auto src_pin_name = pin.rhs_name;
      auto src_pin = get_output_pin_from_def(src_pin_name, def);
      tgt_pin.validate_assign_clause(src_pin, pin.clause);
      pin_map[tgt_pin_name] = {tgt_pin, "scoped"};
    }
  }

  for (const auto &clause : desc->output_assign_clauses) {
    auto tgt_pin_name = clause.name;
    if (pin_map.find(tgt_pin_name) == pin_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "output_assign_clause name not found");
    }
    if (pin_map[tgt_pin_name].meta != "output") {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "output_assign_clause name is not output pin");
    }
    auto tgt_pin = pin_map[tgt_pin_name].pin;
    if (clause.init_by_string) {
      auto init_str = clause.init_str;
      tgt_pin.validate_assign_from_string(init_str, clause.clause);
    } else {
      auto src_pin_name = clause.rhs_name;
      auto src_pin = get_src_pin_from_map(src_pin_name, pin_map);
      tgt_pin.validate_assign_clause(src_pin, clause.clause);
    }
  }

  for (const auto &clause : desc->state_assign_clauses) {
    auto tgt_pin_name = clause.name;
    if (pin_map.find(tgt_pin_name) == pin_map.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "state_assign_clause name not found");
    }
    if (pin_map[tgt_pin_name].meta != "state") {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "state_assign_clause name is not state pin");
    }
    auto tgt_pin = pin_map[tgt_pin_name].pin;
    if (clause.init_by_string) {
      auto init_str = clause.init_str;
      tgt_pin.validate_assign_from_string(init_str, clause.clause);
    } else {
      auto src_pin_name = clause.rhs_name;
      auto src_pin = get_src_pin_from_map(src_pin_name, pin_map);
      tgt_pin.validate_assign_clause(src_pin, clause.clause);
    }
  }
}

contras::pin contras::symbol_definition::get_input_pin_from_def(
    const std::string &pin_name,
    const std::shared_ptr<symbol_definition> &def) {
  for (const auto &pin : def->input_pins) {
    if (pin.name == pin_name) {
      return pin;
    }
  }
  __CONTRAS_THROW(exception_type::invalid_argument, "input_pin name ["
                                                     + pin_name + "] of symbol [" + def->symbol_name + "] not found");
}

contras::pin contras::symbol_definition::get_src_pin_from_map(
    const std::string &pin_name,
    const std::map<std::string, pin_with_meta> &pin_map) {
  auto pin = pin_map.find(pin_name);
  if (pin == pin_map.end()) {
    __CONTRAS_THROW(exception_type::invalid_argument, "src_pin name not found");
  }
  if (pin->second.meta == "output") {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "src_pin cannot be output pin");
  }
  return pin->second.pin;
}

contras::pin contras::symbol_definition::get_output_pin_from_def(
    const std::string &pin_name,
    const std::shared_ptr<symbol_definition> &def) {
  for (const auto &pin : def->output_pins) {
    if (pin.name == pin_name) {
      return pin;
    }
  }
  __CONTRAS_THROW(exception_type::invalid_argument,
                  "output_pin name not found");
}

std::shared_ptr<contras::symbol_definition>
contras::symbol_definition::parse_NAND_definition() {
  auto res = std::make_shared<symbol_definition>();
  res->symbol_name = "NAND";
  res->input_pins.emplace_back("a", 1);
  res->input_pins.emplace_back("b", 1);
  res->output_pins.emplace_back("out", 1);
  return res;
}
