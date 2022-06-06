#ifndef CONTRAS_COMPILER_HPP
#define CONTRAS_COMPILER_HPP

#include "runtime_instance.hpp"
#include "symbol_definition.hpp"
#include "symbol_description.hpp"
#include <map>
#include <set>

namespace contras {

class compiler {
public:
  struct description_node {
    std::shared_ptr<symbol_description> desc;
    std::string file_name, symbol_name;
    std::vector<std::shared_ptr<description_node>> children, parents;
  };

  std::shared_ptr<description_node> description_root;
  std::map<std::string, std::shared_ptr<description_node>> description_nodes;
  std::set<std::string> symbol_name_set, file_name_set;
  std::vector<std::shared_ptr<symbol_description>> sorted_desc;
  std::map<std::string, std::shared_ptr<symbol_definition>> definition_map;

  void dfs_build_description_tree(const std::string &file_name);
  void desc_topological_sort();
  void build_definition_map();
  std::shared_ptr<runtime_instance> build_runtime_instance();
};

} // namespace contras

#endif // CONTRAS_COMPILER_HPP
