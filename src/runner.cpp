#include "runner.hpp"
#include <iostream>

contras::runner::runner(
    std::function<std::shared_ptr<runtime_instance>()> create_inst_,
    std::shared_ptr<test_case> cases_)
    : create_inst(std::move(create_inst_)), cases(std::move(cases_)) {}

void contras::runner::run() {
  for (auto &test_case : cases->test_cases) {
    run_single_test_case(test_case);
  }
}

void contras::runner::run_single_test_case(
    contras::single_test_case &test_case) {
  inst = create_inst();
  for (std::size_t i = 0; i < test_case.length; i++) {
    for (const auto &input : test_case.input_sequences) {
      inst->set_input_pin_from_string(input.pin_name,
                                      {input.pos, input.pos, 0, 0},
                                      input.value[i] == 1 ? "1" : "0");
    }
    inst->execute();
    inst->execute();
    for (auto &output : test_case.output_sequences) {
      output.value[i] = inst->get_output_pin(output.pin_name).value[output.pos];
    }
  }
}

void contras::runner::write_output_file() { cases->print_result(); }
