#ifndef CONTRAS_SYMBOL_DEFINITION_HPP
#define CONTRAS_SYMBOL_DEFINITION_HPP

#include "pin.hpp"
#include "symbol_description.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <functional>

#include <sstream> //used to check nums in the begging of a pin_name

namespace contras
{

  struct symbol_definition
  {
    std::string pin_name;
    std::vector<pin> input_pins, output_pins;
    std::vector<pin_with_default> state_pins;
    std::vector<logic_clause> logic_clauses;
    std::vector<pin_interface_clause> output_assign_clauses, state_assign_clauses;

    static void check_duplicated_pin_names_and_check_if_the_name_is_valid(const std::shared_ptr<symbol_description> &desc)
    {
      std::set<std::string> s;
      // checking inputs :
      for (const auto &input_pin : desc->input_pins)
      {
        if (s.find(input_pin.name) != s.end())
        {
          __CONTRAS_THROW(exception_type::invalid_argument, "message");
        }
        std::stringstream sin(input_pin.name.substr(0, 1));
        int integer_begin;
        if (sin >> integer_begin)
        {
          __CONTRAS_THROW(exception_type::invalid_argument, "message");
        }
        s.insert(input_pin.name);
      }
      // checking outputs :
      for (const auto &output_pin : desc->output_pins)
      {
        if (s.find(output_pin.name) != s.end())
        {
          __CONTRAS_THROW(exception_type::invalid_argument, "message");
        }
        std::stringstream sout(output_pin.name.substr(0, 1));
        int integer_begin;
        if (sout >> integer_begin)
        {
          __CONTRAS_THROW(exception_type::invalid_argument, "message");
        }
        s.insert(output_pin.name);
      }
      // now inpus and outputs are checked and are reserved in set "s"
    }

    void initialize_inputpins_and_outputpins(const std::shared_ptr<symbol_description> &desc)
    {
      for (const auto &input_pin : desc->input_pins)
      {
        input_pins.push_back(input_pin);
      }
      for (const auto &output_pin : desc->output_pins)
      {
        output_pins.push_back(output_pin);
      }
    }

    static void check_logic_clause_symbol_name(const std::shared_ptr<symbol_description> &desc)
    {
      // check if symbols appeared in logic clauses have been imported.
      for (const auto &logic_clause : desc->logic_clauses)
      {
        int flag = 1;
        for (const auto &import_clause : desc->import_clauses)
        {
          if (import_clause.alias != logic_clause.symbol_name)
          {
            continue;
          }
          else
          {
            flag = 0;
          }
        }
        if (flag)
        {
          __CONTRAS_THROW(exception_type::invalid_argument, "message");
        }
      }
      // then check what? def_map?
    }

    pin get_pin_from_def(const std::string &symbol_name, const std::string &pin_name)
    {
      auto it = def_map.find(symbol_name);
      if (it == def_map.end())
      {
        __CONTRAS_THROW(exception_type::invalid_argument, "message");
      }
      auto def = it->second;
      for (const auto &input_pin : def->input_pins)
      {
        if (input_pin.name == pin_name)
          return input_pin;
      }
      __CONTRAS_THROW(exception_type::invalid_argument, "msg");
    }

    void check_pin_interface_clause_in_logic_clauses(const std::shared_ptr<symbol_description> &desc)
    {
      std::map<std::string, pin> input_pins, output_pins, state_pins, scoped_pins;

      auto get_pin_from_scoped =
          [&](const std::string &pin_name)
      {
        auto it1 = input_pins.find(pin_name);
        if (it1 != input_pins.end())
          return it1->second;
        auto it2 = state_pins.find(pin_name);
        if (it2 != state_pins.end())
          return it2->second;
        auto it3 = scoped_pins.find(pin_name);
        if (it3 != scoped_pins.end())
          return it3->second;
        __CONTRAS_THROW(exception_type::invalid_argument, "msg")
      };

      auto vali_duplicated_pin_name =
          [&](const std::string &pin_name)
      {
        if (input_pins.find(pin_name) != input_pins.end()

            || output_pins.find(pin_name) != output_pins.end()

            || state_pins.find(pin_name) != state_pins.end()

            || scoped_pins.find(pin_name) != scoped_pins.end())
        {
          __CONTRAS_THROW(exception_type::invalid_argument, "msg")
        }
      };

      /*
      input, state 是已经初始化过的引脚。
      */
      // first check inputs clauses :
      for (const auto &logic_clause : desc->logic_clauses)
      {
        auto symbol_name = logic_clause.symbol_name;
        for (const auto &input_pin : logic_clause.input_pins)
        {
          auto tgt_pin_name = input_pin.name;
          auto tgt_pin = get_pin_from_def(symbol_name, tgt_pin_name);
          if (input_pin.init_by_string)
          {
            tgt_pin.validate_assign_from_string(input_pin.init_str, input_pin.clause);
          }
          else
          {
            auto src_pin_name = input_pin.rhs_name;
            auto src_pin = get_pin_from_scoped(src_pin_name);
            tgt_pin.validate_assign_clause(src_pin, input_pin.clause);
          }
        }
        for (const auto &output_pin : logic_clause.output_pins)
        {
          auto tgt_pin_name = output_pin.name;
          vali_duplicated_pin_name(tgt_pin_name);
          if (output_pin.init_by_string)
          {
            __CONTRAS_THROW(exception_type::invalid_argument, "msg");
          }
          auto src_pin_name = output_pin.rhs_name;
          auto src_pin = get_pin_from_def(symbol_name, src_pin_name);
          auto tgt_pin = pin(tgt_pin_name, output_pin.clause.r + 1);
          tgt_pin.validate_assign_clause(src_pin, output_pin.clause);
          scoped_pins.insert(std::make_pair(tgt_pin_name, tgt_pin));
        }
      }
    }

    static std::shared_ptr<symbol_definition>
    parse_symbol_definition(const std::shared_ptr<symbol_description> &desc)
    {
      auto res = std::make_shared<symbol_definition>();
      // Exam the name of inputs and outputs of the given pins :
      check_duplicated_pin_names_and_check_if_the_name_is_valid(desc);

      // Initianlize vector "input_pins" and "output_pins" :
      res->initialize_inputpins_and_outputpins(desc);

      // Check the name of symbols appeared in logic clauses :
      check_logic_clause_symbol_name(desc);
    }

  }; // struct symbol_definition

  std::map<std::string, std::shared_ptr<symbol_definition>> def_map;

} // namespace contras

#endif // CONTRAS_SYMBOL_DEFINITION_HPP
