#ifndef CONTRAS_RUNNER_HPP
#define CONTRAS_RUNNER_HPP

#include "runtime_instance.hpp"
#include "test_case.hpp"
#include <memory>

namespace contras {

class runner {
public:
  runner(std::function<std::shared_ptr<runtime_instance>()> create_inst_,
         std::shared_ptr<test_case> cases_);
  void run();
  void run_single_test_case(single_test_case &test_case);
  void write_output_file();

private:
  std::shared_ptr<test_case> cases;
  std::function<std::shared_ptr<runtime_instance>()> create_inst;
  std::shared_ptr<runtime_instance> inst;
};

} // namespace contras

#endif // CONTRAS_RUNNER_HPP
