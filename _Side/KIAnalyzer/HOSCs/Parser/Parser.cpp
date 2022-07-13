#include <sstream>
#include <stdexcept>
#include "Parser.h"
#include "Interpreter.h"
namespace prs
{
    bool Parser::next_line()
    {
        while (true)
        {
            if (ifs_.eof())
                return false;
            std::getline(ifs_, current_line_);
            line_num_++;
            col_num_ = col_num_prev_ = 0;
            remove_comments();
            copy_comment();
            skip_white_space();
            if (!is_end_line())
                return true;
        }
    }

    Parser::Parser(const std::string& in_file_name, const std::string& out_file_name) : in_file_name_(in_file_name), out_file_name_(out_file_name), ifs_(in_file_name), ofs_(out_file_name)
    {
        if (!ifs_.is_open())
        {
            std::ostringstream ss;
            ss << in_file_name_ << " is inaccessible\n";
            throw std::invalid_argument(ss.str());
        }
        if (!ofs_.is_open())
        {
            std::ostringstream ss;
            ss << out_file_name_ << " is inaccessible\n";
            throw std::invalid_argument(ss.str());
        }
        cout_rdbuf_ = std::cout.rdbuf(ofs_.rdbuf());
    }

    Parser::~Parser()
    {
        std::cout.rdbuf(cout_rdbuf_);
        ifs_.close();
        ofs_.close();
    }

    void Parser::remove_comments()
    {
        auto pos_com = current_line_.find("//");
        if (pos_com != std::string::npos)
        {
            current_line_ = current_line_.substr(0, pos_com);
        }
    }

    void Parser::copy_comment()
    {
        auto pos_com = current_line_.find("##");
        if (pos_com != std::string::npos)
        {
            ofs_ << "# " << current_line_.substr(pos_com + 2) << std::endl;
            current_line_ = current_line_.substr(0, pos_com);
        }
    }

    void Parser::run()
    {
        try
        {

            while (next_line())
            {
                // std::cout << "## current line is: " << current_line_ << std::endl;
                while (!is_end_line())
                {
                    if (!complete_literal())
                    {
                        // std::cout << "# Next literal: " << current_statement() << std::endl;
                        interpreter_.push_statement(current_statement());
                        skip_white_space();
                    }
                }
                // while
            }
        }
        catch (const prs::format_error &e)
        {
            std::cout << "## Line: "<< line_num_ << ", Column: " << col_num_prev_ << ". Error: " << e.what() << std::endl;
            // std::cerr << e.what() << '\n';
        }
    }

} // namespace prs