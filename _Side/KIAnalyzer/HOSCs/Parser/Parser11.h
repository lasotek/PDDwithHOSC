/**
 * @file Parser.h
 * @author Sławomir Lasota (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-10-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <memory>
#include "helpers/NodeMaper.h"
#include "HOSCContainer.h"

class parser_error : public std::invalid_argument
{
};

class Parser
{
private:
    std::string input_file_name, output_file_name;
    // std::vector<std::string> in_content;
    void remove_comments(std::string &line);
    /**
     * @brief 
     * 
     * @tparam oper 
     * @param line 
     * @param col 
     * @param _oper 
     * @return true 
     * @return false 
     */
    template <typename oper>
    inline bool complete_statement(const std::string &line, int &col, oper _oper)
    {
        while (col < line.length() && _oper(line[col]))
            col++;
        return col == line.length();
    }
    /**
     * @brief 
     * 
     * @param line stiring line
     * @param col current column
     * @return true if last char
     * @return false otherwise
     */
    inline bool skip_white_space(const std::string &line, int &col)
    {
        return complete_statement(line, col, [](char c)
                                  { return std::isspace(c); });
    }
    inline bool complete_literal(const std::string &line, int &col)
    {
        return complete_statement(line, col, [](char c)
                                  { return c == '_' || std::isalnum(c); });
    }
    inline bool complete_int_number(const std::string &line, int &col)
    {
        return complete_statement(line, col, [](char c)
                                  { return c == '+' || c == '-' || std::isdigit(c); });
    } // inline void comma_division(const std::string &line, int &col)
    // {
    // }

    NodeMapper _node_mapper;
    using EXPECTATIONS = enum { _decl_nodes_expected,
                                _list_of_nodes_expected,
                                _def_expected,
                                _equal_sign_expected,
                                _d_or_HOSC_expected,
                                _HOSC_definition_expected,
                                _o_dot_expected,
                                _o_dot_or_colon_expected,
                                _nodes_to_remove_expected,
                                _pair_expected,
                                _node_expected,
                                _close_bracket_expected,
                                _r_operand_expected,
                                _print_expected
    };
    EXPECTATIONS state = EXPECTATIONS::_decl_nodes_expected;
    std::unordered_map<std::string, std::shared_ptr<HOSCContainer>> _hosc_map;
    inline bool RedefinitionOfHOSC(const std::string &name) const
    {
        return _hosc_map.find(name) != _hosc_map.end();
    }
    inline bool HOSCDefined(const std::string &name) const { return RedefinitionOfHOSC(name); }
    inline auto getExistingHOSC(const std::string &name) const
    {
        auto it = _hosc_map.find(name);
        return (it == _hosc_map.end()) ? nullptr : it->second;
    }
    void print(const std::shared_ptr<HOSCContainer> &ConttoPrint);

public:
    Parser(std::string InputFile, std::string OutputFile);
    ~Parser();
    void parse_file();
};

#endif // __PARSER_H__