/**
 * @file Parser.cpp
 * @author Sławomir Lasota (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <filesystem>
// #include <ostream>
#include "Parser.h"

Parser::Parser(std::string InputFile, std::string OutputFile)
    : input_file_name(InputFile),
      output_file_name(OutputFile)
{
  std::filesystem::remove(OutputFile);
}

Parser::~Parser()
{
}

void Parser::parse_file()
{
  std::ifstream in_stream(input_file_name);
  std::string line;
  int line_no = 0;
  std::shared_ptr<HOSCContainer> sp_currentHOSC;
  // int multiplier{1};
  const std::string one_line{"The definition of any HOSC must be in a single line"};
  const std::string n_undeclared{"Node undeclared!"};

  while (getline(in_stream, line))
  {
    line_no++;
    remove_comments(line);
    for (auto c_no = 0; c_no < line.length(); c_no++)
    {
      auto error_report = [&line, &c_no, line_no](std::string msg)
      {
        std::ostringstream ss;
        ss << line << std::endl;
        ss << std::setw(c_no) << '^' << std::endl;
        ss << "Line: " << line_no << ", Column: " << c_no << " - " << msg << std::endl;
        throw std::logic_error(ss.str().c_str());
      };
      auto nodes_to_remove_extractor = [&](HOSC::nodes_to_remove &_n_list)
      {
        while (line[c_no] != '}')
        {
          if (this->skip_white_space(line, c_no))
            error_report(one_line);
          auto l_counter = c_no;
          complete_literal(line, l_counter);
          auto literal = line.substr(c_no, l_counter - c_no);
          auto n = _node_mapper.name_to_number(literal); //fix it!
          if (n < 0)
            error_report(n_undeclared);
          _n_list.push_back(n);
          c_no = l_counter;
          if (this->skip_white_space(line, c_no))
            error_report(one_line);
          if (line[c_no] == ',')
            c_no++;
        }
      };
      if (skip_white_space(line, c_no))
        break;
      switch (state)
      {
      case _decl_nodes_expected:
      {
        auto command = line.substr(c_no, 5);
        if (command != "nodes")
        {
          error_report("Before any operations, nodes should be defined");
        }
        c_no += 5;
        if (skip_white_space(line, c_no))
          continue;
        if (line[c_no] != '(')
        {
          error_report("\"(\" expected");
        }
        state = _list_of_nodes_expected;
        break;
      }
      case _list_of_nodes_expected:
      {
        auto l_counter = c_no;
        if (skip_white_space(line, c_no))
          break;
        complete_literal(line, l_counter);
        if (!_node_mapper.register_unique_to_number(line.substr(c_no, l_counter - c_no)))
        {
          error_report("Duplicated node declaration!");
        }
        c_no = l_counter;
        if (skip_white_space(line, c_no))
          break;
        auto c = line[c_no];
        if (c != ',' && c != ')')
        {
          error_report("\",\" or \")\" expected");
        }
        if (c == ',')
          break;
        c_no++;
        if (skip_white_space(line, c_no))
          break;
        if (line[c_no] == ';')
        {
          state = _def_expected;
          _node_mapper.lock();
          c_no++;
          break;
        }
        error_report("Any command must be finished by \";\"");
        break;
      }
      case _def_expected:
      {
        auto l_counter = c_no;
        complete_literal(line, l_counter);
        auto literal = line.substr(c_no, l_counter - c_no);
        if (literal == "nodes")
        {
          state = _decl_nodes_expected;
          // _hosc_map.clear();
          _node_mapper.reset();
          c_no--;
          break;
        }
        if (literal == "print")
        {
          c_no = l_counter;
          state = _print_expected;
          break;
        }
        if (RedefinitionOfHOSC(literal))
          error_report("HOSC has already been defined!");
        sp_currentHOSC =
            _hosc_map.emplace(literal, std::make_shared<HOSCContainer>(HOSCContainer(literal, _node_mapper))).first->second;
        state = _equal_sign_expected;
        c_no = l_counter;
        // to be continued
        break;
      }
      case _equal_sign_expected:
        if (skip_white_space(line, c_no))
          break;
        if (line[c_no] != '=')
          error_report("Equal sign expected!");
        state = _d_or_HOSC_expected;
        break;
      case _d_or_HOSC_expected:
      {
        // auto l_counter = c_no;
        // complete_literal(line, l_counter);
        // auto lit = line.substr(c_no, l_counter - c_no);

        //change
        if (line[c_no] == '-')
        {
          // multiplier = -1;
          sp_currentHOSC->set_multiplier(-1);
          c_no++;
        }
        else if (line[c_no] == '(')
        {
          auto l_counter = ++c_no;
          if (complete_int_number(line, l_counter))
            break;
          auto multiplier = std::stoi(line.substr(c_no, l_counter - c_no));
          sp_currentHOSC->set_multiplier(multiplier);
          c_no = l_counter;
          if (line[c_no] != ')')
            error_report("\")\" expected!");
          c_no++;
          if (skip_white_space(line, c_no))
            error_report(one_line);
          if (line[c_no] != '*')
            error_report("\"*\" expected!");
          c_no++;
        }
        if (skip_white_space(line, c_no))
          error_report(one_line);
        auto l_counter = c_no;
        if (complete_literal(line, l_counter))
          error_report(one_line);
        auto lit = line.substr(c_no, l_counter - c_no);
        if (lit == "D")
        {
          state = _pair_expected;
          c_no = l_counter - 1;
        }
        else
        {
          auto firstHOSC = getExistingHOSC(lit);
          if (!firstHOSC)
            error_report("You cannot make any operation with not defined HOSCs!");
          sp_currentHOSC->InitOperations(firstHOSC->getHOSC(), firstHOSC->getLocalMapper());
          c_no = l_counter;
          state = _o_dot_expected;
        }
      }
      break;
      case _print_expected:
      {
        auto l_counter = c_no;
        if (complete_literal(line, l_counter))
          error_report(one_line);
        auto lit = line.substr(c_no, l_counter - c_no);
        auto nextHOSC = getExistingHOSC(lit);
        if (!nextHOSC)
          error_report("You cannot make any operation with not defined HOSCs!");
        if (skip_white_space(line, l_counter))
          error_report(one_line);
        if (line[l_counter] != ';')
          error_report("\";\" expected!");
        c_no = l_counter + 1;
        state = _def_expected;
        print(nextHOSC);
      }
      break;
      case _o_dot_expected:
      {
        if (line[c_no] != '*')
          error_report("\"*\" expected");
        c_no++;
        state = _r_operand_expected;
      }
      break;
      case _o_dot_or_colon_expected:
      {
        if (line[c_no] == '*')
        {
          state = _r_operand_expected;
        }
        else if (line[c_no] == ';')
        {
          sp_currentHOSC->determine_HOSC();
          state = _def_expected;
          sp_currentHOSC.reset();
        }
        else
        {
          error_report("\"*\" or \";\" expected");
        }
        c_no++;
      }
      break;
      case _r_operand_expected:
      {
        auto l_counter = c_no;
        if (complete_literal(line, l_counter))
          error_report(one_line);
        auto lit = line.substr(c_no, l_counter - c_no);
        auto nextHOSC = getExistingHOSC(lit);
        if (!nextHOSC)
          error_report("You cannot make any operation with not defined HOSCs!");
        if (skip_white_space(line, l_counter))
          error_report(one_line);
        auto c = line[l_counter];
        c_no = l_counter;
        switch (c)
        {
        case ';':
          sp_currentHOSC->determine_HOSC();
          state = _def_expected;
          sp_currentHOSC.reset();
          c_no++;
          break;
        case '*':
          c_no++;
          break;
        case '{':
        {
          HOSC::nodes_to_remove _nodes_to_remove;
          c_no++;
          nodes_to_remove_extractor(_nodes_to_remove);
          sp_currentHOSC->_from_formula->AddOperand(nextHOSC->getHOSC(), _nodes_to_remove);
          sp_currentHOSC->correct_nodes(_nodes_to_remove);
          state = _o_dot_or_colon_expected;
        }

        break;
        default:
          error_report("\";\" or \"*\" or \"{\" expected");
        }
      }
      break;
      case _pair_expected:
      {
        auto one_deletion = [&](int &a, int &b)
        {
          if (skip_white_space(line, c_no))
            error_report(one_line);
          if (line[c_no] != '(')
            error_report("\"(\" expected!");
          c_no++;
          auto l_counter = c_no;
          if (complete_literal(line, l_counter))
            error_report(one_line);
          auto lit = line.substr(c_no, l_counter - c_no);
          a = _node_mapper.name_to_number(lit); //fix it
          if (a < 0)
            error_report(n_undeclared);
          c_no = l_counter;
          if (skip_white_space(line, c_no))
            error_report(one_line);
          auto oper = line[c_no++];
          int multiplier{1};
          if (oper == '-')
            multiplier = -1;
          else if (oper != '+')
            error_report("\"+\" OR \"-\" expected");
          if (skip_white_space(line, c_no))
            error_report(one_line);
          l_counter = c_no;
          if (complete_literal(line, l_counter))
            error_report(one_line);
          lit = line.substr(c_no, l_counter - c_no);
          b = _node_mapper.name_to_number(lit); //fix it
          if (b < 0)
            error_report(n_undeclared);
          b *= multiplier;
          c_no = l_counter;
          if (skip_white_space(line, c_no))
            error_report(one_line);
          if (line[c_no] != ')')
            error_report("\")\" expected!");
          c_no++;
        };
        int p, r, k, l;
        one_deletion(p, r);
        one_deletion(k, l);
        //nodes to be continue
        sp_currentHOSC->add_pair_of_deletions(p, r, k, l);
        if (skip_white_space(line, c_no))
          error_report(one_line);
        auto c = line[c_no];
        switch (c)
        {
        case ',':
          break;
        case '{':
        {
          c_no++;
          if (skip_white_space(line, c_no))
            error_report(one_line);
          nodes_to_remove_extractor(sp_currentHOSC->_nodes_to_remove);
          c_no++;
          if (skip_white_space(line, c_no))
            error_report(one_line);
          if (line[c_no] != ';')
            error_report("\";\" expected");
          c_no++;
        }
        case ';':
          sp_currentHOSC->determine_HOSC();
          state = _def_expected;
          sp_currentHOSC.reset();
          break;
        default:
          error_report("\",\" \";\" or \"{\" expected");
        }
      }
      break;
      case _nodes_to_remove_expected:
      {
      }
      break;
      default:
        break;
      }
    }
    // std::remove_if(line.begin(), line.end(),
    //                [](unsigned char c)
    //                {
    //                  return std::isspace(c);
    //                });

    // if (line.length() > 0)
    // {
    //   in_content.push_back(line);
    // }
  }
}

void Parser::remove_comments(std::string &line)
{
  auto pos_com = line.find("//");
  if (pos_com != std::string::npos)
  {
    line = line.substr(0, pos_com);
  }
}

void Parser::print(const std::shared_ptr<HOSCContainer> &ConttoPrint)
{
  std::ofstream out(output_file_name, std::ios::app);
  if (!ConttoPrint->getHOSC())
  {
    out << ConttoPrint->getName() << " = 0\n";
  }
  else
  {
    out << ConttoPrint->getName() << " = "
        << ConttoPrint->getHOSC()->String(&(ConttoPrint->getLocalMapper().Node2Name())) << std::endl;
  }
}
