#ifndef CONTRAS_PIN_HPP
#define CONTRAS_PIN_HPP

#include "utils/exception.hpp"
#include <bitset>
#include <string>

namespace contras {

struct pin_assign_clause {
  std::size_t r, l, rhs_r, rhs_l;
};

struct pin {
  std::string name;
  std::size_t length;
  std::bitset<64> value, initialized;

  pin(std::string name_, std::size_t length_);
  void validate_initialize_str(const std::string &str) const;
  void initialize(const std::string &str);
  void validate_assign_clause(const pin &rhs,
                              const pin_assign_clause &clause) const;
  void assign_value(const pin &rhs, const pin_assign_clause &clause);
  void validate_assign_from_string(const std::string &str,
                                   const pin_assign_clause &clause) const;
  void assign_value_from_string(const std::string &str,
                                const pin_assign_clause &clause);
};

struct pin_with_default : public pin {
  std::string default_value;
  pin_with_default(std::string name_, std::size_t length_,
                   std::string default_value_); // modified
};

} // namespace contras

#endif // CONTRAS_PIN_HPP
