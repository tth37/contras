#ifndef NEPTUNEORM_EXCEPTION_HPP
#define NEPTUNEORM_EXCEPTION_HPP

#include "logger.hpp"
#include <exception>
#include <string>

namespace contras {

enum class exception_type {
  syntax_error = 0,
  invalid_argument = 1,
  runtime_error = 2
};

class exception : public std::exception {
public:
  exception(exception_type type, std::string message);

  ~exception() noexcept override = default;

  std::string message() const noexcept;

  exception_type type() const noexcept;

private:
  exception_type m_type;
  std::string m_message;
};

} // namespace contras

#define __CONTRAS_THROW(type, message)                                         \
  {                                                                            \
    __CONTRAS_LOG(error, message);                                             \
    throw contras::exception(type, message);                                   \
  }

#endif
