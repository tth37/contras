#include "compiler.hpp"
#include <iostream>
#include <queue>

void contras::compiler::dfs_build_description_tree(
    const std::string &file_name) {
  std::cout << "[1/4] Building symbol_description object " + file_name
            << std::endl;
  auto desc = symbol_description::parse_symbol_description(file_name);
  auto desc_node = std::make_shared<description_node>();
  desc_node->desc = desc;
  desc_node->file_name = desc->file_name;
  desc_node->symbol_name = desc->symbol_name;
  if (desc->symbol_name == "") {
    __CONTRAS_THROW(exception_type::invalid_argument, "Symbol name of [" + desc->file_name + "] is empty");
  }
  if (symbol_name_set.find(desc->symbol_name) != symbol_name_set.end()) {
    // __CONTRAS_THROW(exception_type::invalid_argument, "Symbol name [" +
    //                                                        desc->symbol_name +
    //                                                        "] already exists");
    return;
  }
  symbol_name_set.insert(desc->symbol_name);
  if (file_name_set.find(desc->file_name) != file_name_set.end()) {
    __CONTRAS_THROW(exception_type::invalid_argument, "File name [" +
                                                           desc->file_name +
                                                           "] already exists");
  }
  file_name_set.insert(desc->file_name);
  if (description_nodes.empty()) {
    description_root = desc_node;
  }
  if (description_nodes.find(desc->symbol_name) != description_nodes.end()) {
    return;
  }
  description_nodes[desc->symbol_name] = desc_node;
  for (const auto &import_clause : desc->import_clauses) {
    auto child_file_name = import_clause.file_name;
    auto child_symbol_name = import_clause.alias;
    dfs_build_description_tree(child_file_name);
    if (description_nodes.find(child_symbol_name) == description_nodes.end()) {
      __CONTRAS_THROW(exception_type::invalid_argument,
                      "Child symbol_name [" + child_symbol_name + "] not found when parsing [" +
                          desc->symbol_name + "]");
    }
    auto child_desc_node = description_nodes[child_symbol_name];
    desc_node->children.push_back(child_desc_node);
    child_desc_node->parents.push_back(desc_node);
  }
  symbol_name_set.erase(desc->symbol_name);
  file_name_set.erase(desc->file_name);
}

void contras::compiler::desc_topological_sort() {
  std::queue<std::shared_ptr<description_node>> q;
  for (const auto &node : description_nodes) {
    if (node.second->children.empty()) {
      q.push(node.second);
    }
  }
  while (!q.empty()) {
    auto cur_node = q.front();
    q.pop();
    sorted_desc.push_back(cur_node->desc);
    for (const auto &parent : cur_node->parents) {
      auto child_it =
          std::find(parent->children.begin(), parent->children.end(), cur_node);
      if (child_it != parent->children.end()) {
        parent->children.erase(child_it);
      }
      if (parent->children.empty()) {
        q.push(parent);
      }
    }
  }
}

void contras::compiler::build_definition_map() {
  definition_map["NAND"] = symbol_definition::parse_NAND_definition();
  for (const auto &desc : sorted_desc) {
    auto def = symbol_definition::parse_symbol_definition(desc, definition_map);
    std::cout << "[2/4] Building symbol_definition object " + def->symbol_name
              << std::endl;
    definition_map[def->symbol_name] = def;
  }
}

std::shared_ptr<contras::runtime_instance>
contras::compiler::build_runtime_instance() {
  auto top_level_symbol_name = description_root->desc->symbol_name;
  auto top_level_symbol_def = definition_map[top_level_symbol_name];
  return runtime_instance::create_runtime_instance(top_level_symbol_def,
                                                   definition_map);
}
