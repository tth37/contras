#include "symbol_description.hpp"
#include <fstream>
#include <regex>

std::shared_ptr<contras::symbol_description>
contras::symbol_description::parse_symbol_description(
    const std::string &file_name) {
  auto res = std::make_shared<symbol_description>();
  res->file_name = file_name;
  std::ifstream file(file_name);
  if (!file.is_open()) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Cannot open symbol description file [" + file_name + "]");
  }
  std::string line;
  // Before @Head
  for (;;) {
    std::getline(file, line);
    if (file.eof()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Unexpected end of file [" + file_name + "]");
    }
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (line == "@Head") {
      break;
    }
    auto import_clause = parse_import_clause(line);
    res->import_clauses.push_back(import_clause);
  }
  // @Head
  for (;;) {
    std::getline(file, line);
    if (file.eof()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Unexpected end of file [" + file_name + "]");
    }
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (line == "@Body") {
      break;
    }
    if (line.find("NAME") == 0) {
      res->symbol_name = parse_symbol_name(line);
      continue;
    }
    if (line.find("INPUT") == 0) {
      auto input_pins = parse_input_pin(line);
      res->input_pins.insert(res->input_pins.end(), input_pins.begin(),
                             input_pins.end());
      continue;
    }
    if (line.find("OUTPUT") == 0) {
      auto output_pins = parse_output_pin(line);
      res->output_pins.insert(res->output_pins.end(), output_pins.begin(),
                              output_pins.end());
      continue;
    }
    //    if (line.find("STATE") == 0) {
    //      auto state_pins = parse_state_pin(line);
    //      res->state_pins.insert(res->state_pins.end(), state_pins.begin(),
    //                             state_pins.end());
    //      continue;
    //    }
    __CONTRAS_THROW(exception_type::syntax_error,
                    "Unexpected line [" + line + "]");
  }
}

contras::import_clause
contras::symbol_description::parse_import_clause(const std::string &line) {
  auto res = import_clause{};
  // regex: import\s+(\w+)\s+from\s+\"([a-zA-Z0-9\.]+)\"\s*
  std::regex re("import\\s+(\\w+)\\s+from\\s+\"([a-zA-Z0-9\\.]+)\"\\s*");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid import clause [" + line + "]");
  }
  res.alias = match[1];
  res.file_name = match[2];
  return res;
}

std::string
contras::symbol_description::parse_symbol_name(const std::string &line) {
  std::string res;
  // regex: NAME\s+(\w+)\s*
  std::regex re(R"(NAME\s+(\w+)\s*)");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid symbol name [" + line + "]");
  }
  res = match[1];
  __CONTRAS_LOG(debug, "Symbol name: " + res);
  return res;
}

std::vector<contras::pin>
contras::symbol_description::parse_input_pin(const std::string &line) {
  std::vector<pin> res;
  // regex: INPUT\s+([a-zA-Z0-9\[\]\:]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+)\s*)*\s*
  std::regex re(
      R"(INPUT\s+([a-zA-Z0-9\[\]\:]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+)\s*)*\s*)");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid input pin [" + line + "]");
  }
  auto processed_line = line;
  // replace ',' to ' ' in line
  while (processed_line.find(',') != std::string::npos) {
    processed_line.replace(processed_line.find(','), 1, " ");
  }
  std::stringstream ss(processed_line);
  std::string token;
  ss >> token;
  while (ss >> token) {
    auto pin = parse_pin_def(token);
    __CONTRAS_LOG(debug, "Input pin: " + pin.name);
    __CONTRAS_LOG(debug, "Input pin length: " + std::to_string(pin.length));
    res.push_back(pin);
  }

  return res;
}

std::vector<contras::pin>
contras::symbol_description::parse_output_pin(const std::string &line) {
  std::vector<pin> res;
  // regex: OUTPUT\s+([a-zA-Z0-9\[\]\:]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+)\s*)*\s*
  std::regex re(
      R"(OUTPUT\s+([a-zA-Z0-9\[\]\:]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+)\s*)*\s*)");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid output pin [" + line + "]");
  }
  auto processed_line = line;
  // replace ',' to ' ' in line
  while (processed_line.find(',') != std::string::npos) {
    processed_line.replace(processed_line.find(','), 1, " ");
  }
  std::stringstream ss(processed_line);
  std::string token;
  ss >> token;
  while (ss >> token) {
    auto pin = parse_pin_def(token);
    __CONTRAS_LOG(debug, "Output pin: " + pin.name);
    __CONTRAS_LOG(debug, "Output pin length: " + std::to_string(pin.length));
    res.push_back(pin);
  }

  return res;
}

contras::pin
contras::symbol_description::parse_pin_def(const std::string &str) {
  // regex pattern 1: [a-zA-Z0-9]+
  std::regex re1(R"([a-zA-Z0-9]+)");
  // regex pattern 2: ([a-zA-Z0-9]+\[[0-9]+\:[0-9]+\])
  std::regex re2(R"(([a-zA-Z0-9]+)\[[0-9]+\:[0-9]+\])");
  std::smatch match;
  if (std::regex_match(str, match, re1)) {
    return pin{str, 1};
  }
  if (std::regex_match(str, match, re2)) {
    auto processed_str = str;
    while (processed_str.find('[') != std::string::npos) {
      processed_str.replace(processed_str.find('['), 1, " ");
    }
    while (processed_str.find(']') != std::string::npos) {
      processed_str.replace(processed_str.find(']'), 1, " ");
    }
    while (processed_str.find(':') != std::string::npos) {
      processed_str.replace(processed_str.find(':'), 1, " ");
    }
    std::stringstream ss(processed_str);
    std::string token;
    ss >> token;
    std::string pin_name = token;
    size_t right_pos, left_pos;
    ss >> right_pos >> left_pos;
    if (left_pos != 0) {
      __CONTRAS_THROW(exception_type::syntax_error,
                      "Invalid pin definition [" + str + "]");
    }
    return pin{pin_name, right_pos - left_pos + 1};
  }
  __CONTRAS_THROW(exception_type::syntax_error,
                  "Invalid pin definition [" + str + "]");
}
