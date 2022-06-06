#include "pin.hpp"

contras::pin::pin(std::string name_, std::size_t length_)
    : name(std::move(name_)), length(length_) {
  if (length > 64) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Pin length must be less than 64");
  }
}

void contras::pin::validate_initialize_str(const std::string &str) const {
  if (str.length() != 1 && str.length() != length) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN initialize string");
  }
  for (auto c : str) {
    if (c != '0' && c != '1') {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Invalid PIN initialize string");
    }
  }
}

void contras::pin::initialize(const std::string &str) {
  validate_initialize_str(str);
  if (str.length() == 1) {
    for (std::size_t i = 0; i < length; ++i) {
      value[i] = str[0] == '1';
      initialized[i] = true;
    }
  } else {
    for (std::size_t i = 0; i < length; ++i) {
      value[i] = str[i] == '1';
      initialized[i] = true;
    }
  }
}

void contras::pin::validate_assign_clause(
    const pin &rhs, const pin_assign_clause &clause) const {
  if (clause.r >= length || clause.rhs_r >= rhs.length || clause.l < 0 ||
      clause.rhs_l < 0) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN assign clause");
  }
  if (clause.l > clause.r || clause.rhs_l > clause.rhs_r) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN assign clause");
  }
  if (clause.r - clause.l != clause.rhs_r - clause.rhs_l) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN assign clause");
  }
}

void contras::pin::assign_value(const pin &rhs,
                                const pin_assign_clause &clause) {
  validate_assign_clause(rhs, clause);
  bool used_uninitialized_value = false;

  for (std::size_t i = clause.l; i <= clause.r; ++i) {
    value[i] = rhs.value[clause.rhs_r - (clause.r - i)];
    if (!rhs.initialized[clause.rhs_r - (clause.r - i)]) {
      used_uninitialized_value = true;
    }
    initialized[i] = true;
  }

  if (used_uninitialized_value) {
    __CONTRAS_LOG(warn, "Use of uninitialized value in PIN assign clause");
  }
}

void contras::pin::validate_assign_from_string(
    const std::string &str, const contras::pin_assign_clause &clause) const {
  if (clause.r >= length || clause.l < 0) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN assign clause");
  }
  if (clause.l > clause.r) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN assign clause");
  }
  std::size_t clause_length = clause.r - clause.l + 1;
  if (str.length() != clause_length && str.length() != 1) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid PIN assign clause");
  }
  for (auto c : str) {
    if (c != '0' && c != '1') {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Invalid PIN assign clause");
    }
  }
}

void contras::pin::assign_value_from_string(
    const std::string &str, const contras::pin_assign_clause &clause) {
  validate_assign_from_string(str, clause);
  if (str.length() == 1) {
    for (std::size_t i = clause.l; i <= clause.r; ++i) {
      value[i] = str[0] == '1';
      initialized[i] = true;
    }
  } else {
    for (std::size_t i = clause.l; i <= clause.r; ++i) {
      value[i] = str[i - clause.l] == '1';
      initialized[i] = true;
    }
  }
}

void contras::pin::clear() {
  for (std::size_t i = 0; i < length; ++i) {
    value[i] = false;
    initialized[i] = false;
  }
}

contras::pin::pin() : length(0), value(0), initialized(0) {}

contras::pin_with_default::pin_with_default(std::string name_,
                                            std::size_t length_,
                                            std::string default_value_)
    : pin(std::move(name_), length_), default_value(std::move(default_value_)) {
  validate_initialize_str(default_value);
}