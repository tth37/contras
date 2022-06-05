#ifndef CONTRAS_LOGGER_HPP
#define CONTRAS_LOGGER_HPP

#include <mutex>
#include <string>
#include <memory>

namespace contras {

class logger {
public:
  void debug(const std::string &message, const std::string &file,
             std::size_t line);

  void info(const std::string &message, const std::string &file,
            std::size_t line);

  void warn(const std::string &message, const std::string &file,
            std::size_t line);

  void error(const std::string &message, const std::string &file,
             std::size_t line);

private:
  std::mutex m_mtx;
};

void use_logger();

void debug(const std::string &message, const std::string &file,
           std::size_t line);

void info(const std::string &message, const std::string &file,
          std::size_t line);

void warn(const std::string &message, const std::string &file,
          std::size_t line);

void error(const std::string &message, const std::string &file,
           std::size_t line);

#ifdef __CONTRAS_LOGGER_DISABLED
#define __CONTRAS_LOG(level, msg)
#else
#define __CONTRAS_LOG(level, msg)                                              \
  { contras::level(msg, __FILE__, __LINE__); }
#endif

} // namespace contras

#endif
