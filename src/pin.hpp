#ifndef CONTRAS_PIN_HPP
#define CONTRAS_PIN_HPP

#include "utils/exception.hpp"
#include <bitset>
#include <string>

namespace contras {

struct pin_assign_clause {
  std::size_t r, l, rhs_r, rhs_l; //two pins's left marks and right marks. r and l belong to self. rhs_r and rhs_l belong to ready-to-be-assigned pin
};

struct pin {
  std::string name;
  std::size_t length;
  std::bitset<64> value, initialized;

  pin(std::string name_, std::size_t length_);//modified

  void validate_initialize_str(const std::string &str) const;//modified

  void initialize(const std::string &str);//modified

  void validate_assign_clause(const pin &rhs, const pin_assign_clause &clause) const;//modified

  void assign_value(const pin &rhs, const pin_assign_clause &clause);//modified
};

struct pin_with_default : public pin {
  std::string default_value;
  pin_with_default(std::string name_, std::size_t length_, std::string default_value_);//modified
};

} // namespace contras

#endif // CONTRAS_PIN_HPP
