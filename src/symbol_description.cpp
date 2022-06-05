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
    if (file.eof()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Unexpected end of file [" + file_name + "]");
    }
    std::getline(file, line);
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
    if (file.eof()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Unexpected end of file [" + file_name + "]");
    }
    std::getline(file, line);
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
    if (line.find("STATE") == 0) {
      auto state_pins = parse_state_pin(line);
      res->state_pins.insert(res->state_pins.end(), state_pins.begin(),
                             state_pins.end());
      continue;
    }
    __CONTRAS_THROW(exception_type::syntax_error,
                    "Unexpected line [" + line + "]");
  }
  // @Body
  for (;;) {
    if (file.eof()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Unexpected end of file [" + file_name + "]");
    }
    std::getline(file, line);
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (line == "@End") {
      break;
    }
    auto logic_clause = parse_logic_clause(line);
    res->logic_clauses.push_back(logic_clause);
  }
  // @End
  for (;;) {
    if (file.eof()) {
      break;
    }
    std::getline(file, line);
    if (line.empty() || line[0] == '#') {
      continue;
    }
    if (line == "@End") {
      break;
    }
    if (line.find("OUTPUT") == 0) {
      auto output_assign_clauses = parse_output_assign_clause(line);
      res->output_assign_clauses.insert(res->output_assign_clauses.end(),
                                        output_assign_clauses.begin(),
                                        output_assign_clauses.end());
      continue;
    }
    if (line.find("STATE") == 0) {
      auto state_assign_clauses = parse_state_assign_clause(line);
      res->state_assign_clauses.insert(res->state_assign_clauses.end(),
                                       state_assign_clauses.begin(),
                                       state_assign_clauses.end());
      continue;
    }
    __CONTRAS_THROW(exception_type::syntax_error,
                    "Unexpected line [" + line + "]");
  }
  return res;
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

contras::pin_with_default
contras::symbol_description::parse_pin_with_default_def(
    const std::string &str, const std::string &init_str) {
  auto pin = parse_pin_def(str);
  pin.validate_initialize_str(init_str);
  return pin_with_default{pin.name, pin.length, init_str};
}

std::vector<contras::pin_with_default>
contras::symbol_description::parse_state_pin(const std::string &line) {
  std::vector<pin_with_default> res;
  // regex:
  // STATE\s+([a-zA-Z0-9\[\]\:]+\s*\=\s*[01]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+\s*\=\s*[01]+)\s*)*\s*
  std::regex re(
      R"(STATE\s+([a-zA-Z0-9\[\]\:]+\s*\=\s*[01]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+\s*\=\s*[01]+)\s*)*\s*)");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::invalid_argument,
                    "Invalid state pin [" + line + "]");
  }
  auto processed_line = line;
  // replace ',' to ' ' in line
  while (processed_line.find(',') != std::string::npos) {
    processed_line.replace(processed_line.find(','), 1, " ");
  }
  // replace '=' to ' ' in line
  while (processed_line.find('=') != std::string::npos) {
    processed_line.replace(processed_line.find('='), 1, " ");
  }
  std::stringstream ss(processed_line);
  std::string token, init_str;
  ss >> token;
  while (ss >> token) {
    ss >> init_str;
    auto pin = parse_pin_with_default_def(token, init_str);
    __CONTRAS_LOG(debug, "State pin: " + pin.name);
    __CONTRAS_LOG(debug, "State pin length: " + std::to_string(pin.length));
    __CONTRAS_LOG(debug, "State pin default value: " + pin.default_value);
    res.push_back(pin);
  }
  return res;
}

contras::pin_interface_clause
contras::symbol_description::parse_pin_interface_clause(
    const std::string &str, const std::string &rhs_str) {
  auto res = pin_interface_clause{};
  // regex: [01]+
  std::regex re("[01]+");
  std::smatch match;
  if (std::regex_match(rhs_str, match, re)) {
    res.init_by_string = true;
    res.init_str = rhs_str;
    auto [pin_name, pin_r, pin_l] = parse_pin_assign_clause_one_side(str);
    res.name = pin_name;
    res.clause.r = pin_r;
    res.clause.l = pin_l;
    __CONTRAS_LOG(debug, "Pin interface clause: " + res.name);
    __CONTRAS_LOG(debug, "Pin interface clause right pos: " +
                             std::to_string(res.clause.r));
    __CONTRAS_LOG(debug, "Pin interface clause left pos: " +
                             std::to_string(res.clause.l));
    __CONTRAS_LOG(debug, "Pin interface clause init value: " + res.init_str);
    return res;
  } else {
    res.init_by_string = false;
    auto [pin_name, pin_r, pin_l] = parse_pin_assign_clause_one_side(str);
    auto [rhs_pin_name, rhs_pin_r, rhs_pin_l] =
        parse_pin_assign_clause_one_side(rhs_str);
    res.name = pin_name;
    res.clause.r = pin_r;
    res.clause.l = pin_l;
    res.rhs_name = rhs_pin_name;
    res.clause.rhs_r = rhs_pin_r;
    res.clause.rhs_l = rhs_pin_l;
    return res;
  }
  __CONTRAS_THROW(exception_type::invalid_argument,
                  "Invalid pin interface clause [" + str + "]");
}

std::tuple<std::string, std::size_t, std::size_t>
contras::symbol_description::parse_pin_assign_clause_one_side(
    const std::string &str) {
  // regex pattern 1: [a-zA-Z0-9]+
  // regex pattern 2: ([a-zA-Z0-9]+\[[0-9]+\:[0-9]+\])
  // regex pattern 3: ([a-zA-Z0-9]+\[[0-9]+\])
  std::regex re1(R"([a-zA-Z0-9]+)");
  std::regex re2(R"(([a-zA-Z0-9]+)\[[0-9]+\:[0-9]+\])");
  std::regex re3(R"(([a-zA-Z0-9]+)\[[0-9]+\])");
  std::smatch match;
  if (std::regex_match(str, match, re1)) {
    return std::make_tuple(str, 0, 0);
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
    return std::make_tuple(pin_name, right_pos, left_pos);
  }
  if (std::regex_match(str, match, re3)) {
    auto processed_str = str;
    while (processed_str.find('[') != std::string::npos) {
      processed_str.replace(processed_str.find('['), 1, " ");
    }
    while (processed_str.find(']') != std::string::npos) {
      processed_str.replace(processed_str.find(']'), 1, " ");
    }
    std::stringstream ss(processed_str);
    std::string token;
    ss >> token;
    std::string pin_name = token;
    size_t right_pos;
    ss >> right_pos;
    return std::make_tuple(pin_name, right_pos, right_pos);
  }
  __CONTRAS_THROW(exception_type::syntax_error,
                  "Invalid pin assign clause [" + str + "]");
}

contras::logic_clause
contras::symbol_description::parse_logic_clause(const std::string &line) {
  auto res = logic_clause();
  // regex:
  // [a-zA-Z0-9]+\s*\(\s*([a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+)\s*)*\s*\)\s*\=\>\s*\(\s*([a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+)(\s*,\s*([a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+)\s*)*\s*\)\s*
  std::regex re(
      "[a-zA-Z0-9]+\\s*\\(\\s*([a-zA-Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-zA-"
      "Z0-9\\[\\]\\:]+)(\\s*,\\s*([a-zA-Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-"
      "zA-Z0-9\\[\\]\\:]+))*\\s*\\)\\s*\\=\\>\\s*\\(\\s*([a-zA-Z0-"
      "9\\[\\]\\:]+\\s*\\=\\s*[a-zA-Z0-9\\[\\]\\:]+)(\\s*,\\s*([a-zA-"
      "Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-zA-Z0-9\\[\\]\\:]+))*\\s*\\)\\s*");

  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::syntax_error,
                    "Invalid logic clause [" + line + "]");
  }
  auto processed_line = line;
  while (processed_line.find('(') != std::string::npos) {
    processed_line.replace(processed_line.find('('), 1, " ");
  }
  while (processed_line.find(')') != std::string::npos) {
    processed_line.replace(processed_line.find(')'), 1, " ");
  }
  while (processed_line.find('=') != std::string::npos) {
    processed_line.replace(processed_line.find('='), 1, " ");
  }
  while (processed_line.find(',') != std::string::npos) {
    processed_line.replace(processed_line.find(','), 1, " ");
  }
  // divide processed_line into two parts, divided by "=>"
  std::stringstream ss(processed_line);
  std::string part1, part2;
  auto div_pos = processed_line.find('>');
  part1 = processed_line.substr(0, div_pos);
  part2 = processed_line.substr(div_pos + 1);

  std::string token, assign_str;
  std::stringstream ss1(part1);
  ss1 >> token;
  res.symbol_name = token;
  while (ss1 >> token) {
    ss1 >> assign_str;
    auto pin_interface_clause = parse_pin_interface_clause(token, assign_str);
    res.input_pins.push_back(pin_interface_clause);
    __CONTRAS_LOG(
        debug,
        "Input pin: " + pin_interface_clause.name +
            " Right pos: " + std::to_string(pin_interface_clause.clause.r) +
            " Left pos: " + std::to_string(pin_interface_clause.clause.l));
  }
  std::stringstream ss2(part2);
  while (ss2 >> token) {
    ss2 >> assign_str;
    auto pin_interface_clause = parse_pin_interface_clause(token, assign_str);
    res.output_pins.push_back(pin_interface_clause);
    __CONTRAS_LOG(
        debug,
        "Output pin: " + pin_interface_clause.name +
            " Right pos: " + std::to_string(pin_interface_clause.clause.r) +
            " Left pos: " + std::to_string(pin_interface_clause.clause.l));
  }
  return res;
}

std::vector<contras::pin_interface_clause>
contras::symbol_description::parse_output_assign_clause(
    const std::string &line) {
  std::vector<pin_interface_clause> res;
  // regex:
  // OUTPUT\s+([a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+)(\s*\,\s*[a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+\s*)*\s*
  std::regex re(
      "OUTPUT\\s+([a-zA-Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-zA-Z0-9\\[\\]\\:]+)(\\s*"
      "\\,\\s*[a-zA-Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-zA-Z0-9\\[\\]\\:]+\\s*)*\\s*");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::syntax_error,
                    "Invalid output assign clause [" + line + "]");
  }
  auto processed_line = line;
  // replace ',' with ' '
  while (processed_line.find(',') != std::string::npos) {
    processed_line.replace(processed_line.find(','), 1, " ");
  }
  // replace '=' with ' '
  while (processed_line.find('=') != std::string::npos) {
    processed_line.replace(processed_line.find('='), 1, " ");
  }

  std::stringstream ss(processed_line);
  std::string token, assign_str;
  ss >> token;
  while (ss >> token) {
    ss >> assign_str;
    auto pin_interface_clause = parse_pin_interface_clause(token, assign_str);
    res.push_back(pin_interface_clause);
    __CONTRAS_LOG(
        debug,
        "Output pin: " + pin_interface_clause.name +
            " Right pos: " + std::to_string(pin_interface_clause.clause.r) +
            " Left pos: " + std::to_string(pin_interface_clause.clause.l));
  }
  return res;
}

std::vector<contras::pin_interface_clause>
contras::symbol_description::parse_state_assign_clause(
    const std::string &line) {
  std::vector<pin_interface_clause> res;
  // regex:
  // STATE\s+([a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+)(\s*\,\s*[a-zA-Z0-9\[\]\:]+\s*\=\s*[a-zA-Z0-9\[\]\:]+\s*)*\s*
  std::regex re(
      "STATE\\s+([a-zA-Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-zA-Z0-9\\[\\]\\:]+)(\\s*"
      "\\,\\s*[a-zA-Z0-9\\[\\]\\:]+\\s*\\=\\s*[a-zA-Z0-9\\[\\]\\:]+\\s*)*\\s*");
  std::smatch match;
  if (!std::regex_match(line, match, re)) {
    __CONTRAS_THROW(exception_type::syntax_error,
                    "Invalid state assign clause [" + line + "]");
  }
  auto processed_line = line;
  // replace ',' with ' '
  while (processed_line.find(',') != std::string::npos) {
    processed_line.replace(processed_line.find(','), 1, " ");
  }
  // replace '=' with ' '
  while (processed_line.find('=') != std::string::npos) {
    processed_line.replace(processed_line.find('='), 1, " ");
  }

  std::stringstream ss(processed_line);
  std::string token, assign_str;
  ss >> token;
  while (ss >> token) {
    ss >> assign_str;
    auto pin_interface_clause = parse_pin_interface_clause(token, assign_str);
    res.push_back(pin_interface_clause);
    __CONTRAS_LOG(
        debug,
        "State pin: " + pin_interface_clause.name +
            " Right pos: " + std::to_string(pin_interface_clause.clause.r) +
            " Left pos: " + std::to_string(pin_interface_clause.clause.l));
  }
  return res;
}
