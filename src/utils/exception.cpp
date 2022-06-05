#include "exception.hpp"

contras::exception::exception(exception_type type, std::string message)
    : m_type(type), m_message(std::move(message)) {}

std::string contras::exception::message() const noexcept { return m_message; }

contras::exception_type contras::exception::type() const noexcept {
  return m_type;
}
