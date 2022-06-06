#ifndef CONTRAS_TEST_CASE_HPP
#define CONTRAS_TEST_CASE_HPP

#include <memory>
#include <string>
#include <vector>

namespace contras {

struct boolean_sequence {
  std::string pin_name;
  std::size_t pos;
  std::vector<bool> value;
};

struct single_test_case {
  std::size_t length;
  std::vector<boolean_sequence> input_sequences;
  std::vector<boolean_sequence> output_sequences;
};

struct test_case {
  std::vector<single_test_case> test_cases;
  std::string output_file_name;

  static std::shared_ptr<test_case>
  parse_test_case(const std::string &file_name,
                  const std::string &output_file_name);
  void print_result() const;
};

} // namespace contras

#endif // CONTRAS_TEST_CASE_HPP
