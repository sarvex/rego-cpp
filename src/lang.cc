#include "lang.h"

#include "math.h"
#include "passes.h"
#include "wf.h"

#include <charconv>

namespace rego
{
  Node merge_lists(const Node& lhs, const Node& rhs)
  {
    Node list = NodeDef::create(TopKeyValueList);
    std::set<std::string> keys;
    for (auto kv : *lhs)
    {
      std::string key(kv->front()->location().view());
      keys.insert(key);
      list->push_back(kv);
    }

    for (auto kv : *rhs)
    {
      std::string key(kv->front()->location().view());
      if (keys.count(key))
      {
        return err(rhs, "Merge error");
      }

      list->push_back(kv);
    }

    return list;
  }

  std::vector<PassCheck> passes()
  {
    return {
      {"input_data", input_data(), wf_pass_input_data},
      {"modules", modules(), wf_pass_modules},
      {"lists", lists(), wf_pass_lists},
      {"structure", structure(), wf_pass_structure},
      {"multiply_divide", multiply_divide(), wf_pass_multiply_divide},
      {"add_subtract", add_subtract(), wf_pass_add_subtract},
      {"comparison", comparison(), wf_pass_comparison},
      {"merge_data", merge_data(), wf_pass_merge_data},
      {"merge_modules", merge_modules(), wf_pass_merge_modules},
      {"rules", rules(), wf_pass_rules},
      {"convert_modules", convert_modules(), wf_pass_convert_modules},
      {"query", query(), wf_pass_query},
    };
  }

  Driver& driver()
  {
    auto passdefs = passes();
    static Driver d(
      "rego",
      parser(),
      wf_parser,
      {
        {"input_data", input_data(), wf_pass_input_data},
        {"modules", modules(), wf_pass_modules},
        {"lists", lists(), wf_pass_lists},
        {"structure", structure(), wf_pass_structure},
        {"multiply_divide", multiply_divide(), wf_pass_multiply_divide},
        {"add_subtract", add_subtract(), wf_pass_add_subtract},
        {"comparison", comparison(), wf_pass_comparison},
        {"merge_data", merge_data(), wf_pass_merge_data},
        {"merge_modules", merge_modules(), wf_pass_merge_modules},
        {"rules", rules(), wf_pass_rules},
        {"convert_modules", convert_modules(), wf_pass_convert_modules},
        {"query", query(), wf_pass_query},
      });
    return d;
  }

  std::string to_json(const Node& node)
  {
    std::stringstream buf;
    if (node->type() == Int)
    {
      buf << get_int(node);
    }
    else if (node->type() == Float)
    {
      buf << get_double(node);
    }
    else if (node->type() == String)
    {
      buf << node->location().view();
    }
    else if (node->type() == Key)
    {
      buf << '"' << node->location().view() << '"';
    }
    else if (node->type() == Bool)
    {
      if (get_bool(node))
      {
        buf << "true";
      }
      else
      {
        buf << "false";
      }
    }
    else if (node->type() == Null)
    {
      buf << "null";
    }
    else if (node->type() == Undefined)
    {
      buf << "undefined";
    }
    else if (node->type() == Array)
    {
      buf << "[";
      std::string sep = "";
      for (const auto& child : *node)
      {
        buf << sep << to_json(child);
        sep = ", ";
      }
      buf << "]";
    }
    else if (node->type() == KeyValue)
    {
      auto key = node->front();
      auto value = node->back();
      buf << to_json(key) << ": " << to_json(value);
    }
    else if (node->type() == Object)
    {
      buf << "{";
      std::string sep = "";
      for (const auto& child : *node)
      {
        buf << sep << to_json(child);
        sep = ", ";
      }
      buf << "}";
    }
    else
    {
      return "unsupported node type";
    }

    return buf.str();
  }
}