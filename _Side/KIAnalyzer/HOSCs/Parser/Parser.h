#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <fstream>
#include <iostream>
#include "Interpreter.h"

namespace prs
{

    class Parser
    {
    private:
        std::string in_file_name_;
        std::string out_file_name_;
        std::ifstream ifs_;
        std::ofstream ofs_;
        std::string current_line_;
        std::streambuf *cout_rdbuf_;
        size_t line_num_ = 0, col_num_ = 0, col_num_prev_ = 0;
        Interpreter interpreter_;

        /**
         * @brief Check if the cluster is already defined
         * 
         * @param cluster_name cluster name
         * @return true is defined
         * @return false is not defined
         */

        bool next_line();

        /**
         * @brief skip comments in the current line
         *
         * @param line current line
         */
        void remove_comments();

        /**
         * @brief copy copyable comments to the output file
         *
         * @param line current line
         */

        void copy_comment();

        /**
         * @brief
         *
         * @tparam oper
         * @param _oper
         * @return true
         * @return false
         */
        template <typename oper>
        inline bool complete_statement(oper _oper)
        {
            col_num_prev_ = col_num_;
            while (col_num_ < current_line_.length() && _oper(current_line_[col_num_]))
                col_num_++;
            return col_num_prev_ == current_line_.length();
            // return col_num_prev_ == col_num_;
        }

        inline bool skip_white_space(bool next = false)
        {
            if (next)
                col_num_++;
            return complete_statement([](char c)
                                      { return std::isspace(c); });
        }
        inline bool complete_literal()
        {
            return complete_statement([](char c)
                                      { return c == '_' || c == '.' || c=='-' || c== '+'|| std::isalnum(c); });
        }
        inline bool complete_int_number()
        {
            return complete_statement([](char c)
                                      { return c == '+' || c == '-' || std::isdigit(c); });
        }

        inline std::string current_statement()
        {
            if(col_num_prev_ == col_num_ && col_num_ < current_line_.length())
                col_num_++;
            return current_line_.substr(col_num_prev_, col_num_ - col_num_prev_);
        }
        inline bool is_end_line()
        {
            return col_num_prev_ >= current_line_.length();
        }

    public:
        /**
         * @brief Construct a new Parser object
         *
         * @param in_file_name
         * @param out_file_name
         */
        Parser(const std::string& in_file_name, const std::string& out_file_name);

        /**
         * @brief Destroy the Parser object
         *
         */
        ~Parser();

        /**
         * @brief
         *
         * @return std::ostream &
         */
        operator std::ostream &() { return ofs_; }

        /**
         * @brief
         *
         */
        void run();
    };

} // namespace prs

#endif // __PARSER_H__