#include "test_case.hpp"
#include "utils/exception.hpp"
#include <fstream>
#include <regex>
#include <iomanip>
#include <iostream>

std::shared_ptr<contras::test_case>
contras::test_case::parse_test_case(const std::string &file_name,
                                    const std::string &output_file_name) {
  auto res = std::make_shared<test_case>();
  res->output_file_name = output_file_name;

  std::regex case_start("@Case");
  std::regex case_end("@End");
  std::regex bool_sentence(R"([a-zA-Z0-9\[\]]+\s*\=\s*[01]+\s*)");
  std::regex question_mark_sentence(R"([a-zA-Z0-9\[\]]+\s*\=\s*\?\s*)");
  std::regex only_one_and_zero_str("[01]+");
  std::regex match_pin_name("[a-zA-Z0-9]+");
  std::regex match_pin_pos("\\[[0-9]+\\]");

  std::ifstream inFile;
  inFile.open(file_name);
  int count_case = 0;

  int previousLength = 0;

  contras::single_test_case one_test_case;
  for (std::string one_line; std::getline(
           inFile, one_line);) { // every loop get one line from file "test.in"
    if (std::regex_search(one_line, case_start)) { //@Case
      ++count_case;
      previousLength = 0;
      one_test_case.input_sequences.clear();
      one_test_case.output_sequences.clear();
      one_test_case.length = 0;
      continue;
    } else if (std::regex_search(one_line, case_end)) { //@End
      res->test_cases.push_back(one_test_case); // TODO to be modified...
      continue;
    }

    // In the one line of the case
    contras::boolean_sequence one_bool_sequence;
    std::smatch match_res;

    if (std::regex_match(one_line, bool_sentence)) { // is input sequence

            std::vector<std::string> nums_in_sequence;
            std::string tempStr = one_line;
            while (regex_search(tempStr,match_res,only_one_and_zero_str)) {
                nums_in_sequence.push_back(match_res.str(0));
                tempStr = match_res.suffix().str();
            }


      // now get the 01sequence

      std::string bool_seq = nums_in_sequence.back();

      // length judgement
      if (previousLength == 0) {
        previousLength = bool_seq.length();
        one_test_case.length = previousLength;
      } else if (bool_seq.length() != one_test_case.length)
        __CONTRAS_THROW(contras::exception_type::syntax_error,
                        "input sequences' lengths are not equal!")

      for (auto ch : bool_seq) { // store the one_zero_string into
                                 // vector "one_bool_sequence.value"
        if (ch == ' ')
          continue;
        else {
          if (ch == '0') {
            one_bool_sequence.value.push_back(false);
          } else if (ch == '1') {
            one_bool_sequence.value.push_back(true);
          } else {
            __CONTRAS_THROW(contras::exception_type::syntax_error,
                            "01sequence shouldn't include other nums!");
          }
        }
      } // end for

      // now get the pin name and the pos
      std::regex_search(one_line, match_res, match_pin_name);
      one_bool_sequence.pin_name = match_res[0].str();
      if (std::regex_search(one_line, match_res, match_pin_pos)) {
        std::string pos_str = match_res[0].str();
        pos_str = pos_str.substr(1, pos_str.length() - 2);
        one_bool_sequence.pos = std::stoi(pos_str);
      } else {
        one_bool_sequence.pos = 0;
      }

      // now the one_bool_sequence of the current line is done and we push it
      // into "one_test_case.input_sequences"
      one_test_case.input_sequences.push_back(one_bool_sequence);
    } else if (std::regex_match(one_line,
                                question_mark_sentence)) { // is in ?sequence
      // now get the pin name and the pos
      std::regex_search(one_line, match_res, match_pin_name);
      one_bool_sequence.pin_name = match_res[0].str();
      if (std::regex_search(one_line, match_res, match_pin_pos)) {
        one_bool_sequence.pos = std::stoi(match_res[0].str());
      } else {
        one_bool_sequence.pos = 0;//TODO modified fron null to 0
      }
      // now the one_bool_sequence of the current line is done and we push it
      // into "one_test_case.output_sequences"
      one_test_case.output_sequences.push_back(one_bool_sequence);
    } else {
      __CONTRAS_THROW(contras::exception_type::syntax_error,
                      "regex cannot match the input/output sequence in case " +
                          std::to_string(count_case));
    }
  } // end for and all the info is stored in vector "test_cases"

  inFile.close();

  for (auto &test_case : res->test_cases) {
    for (auto &output_sequence : test_case.output_sequences) {
      for (std::size_t i = 0; i < test_case.length; ++i) {
        output_sequence.value.push_back(false);
      }
    }
  }

  return res;
}

void print_io_sequence(contras::boolean_sequence io_seq,
                       std::fstream &outfile, std::size_t width) { 
  outfile << std::setw(width) << io_seq.pin_name + "[" + std::to_string(io_seq.pos) + "]";
  outfile << " = ";
  for (const auto &ch : io_seq.value) {
    if (ch == true)
      outfile << 1;
    else
      outfile << 0;
  }
  outfile << std::endl;
}

void contras::test_case::print_result() const {
  // loop through every single_test_casein test_cases
  // for every single_test_case,loop through input_sequences and
  // output_sequences;

  std::fstream out_File;
  out_File.open(output_file_name, std::ios::out);

  for (const auto &singleCase : test_cases) {
    out_File << "@Case" << std::endl;
    std::size_t width = 0;
    for (const auto &inputSeq : singleCase.input_sequences) {
      auto str = inputSeq.pin_name + "[" + std::to_string(inputSeq.pos) + "]";
      if (str.length() > width) width = str.length();
    }
    for (const auto &outputSeq : singleCase.output_sequences) {
      auto str = outputSeq.pin_name + "[" + std::to_string(outputSeq.pos) + "]";
      if (str.length() > width) width = str.length();
    }
    for (const auto &inputSeq : singleCase.input_sequences) {
      print_io_sequence(inputSeq, out_File, width);
    }
    for (const auto &outputSeq : singleCase.output_sequences) {
      print_io_sequence(outputSeq, out_File, width);
    }
  }

  out_File.close();
}