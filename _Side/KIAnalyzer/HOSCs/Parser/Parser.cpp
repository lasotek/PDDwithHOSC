#include <sstream>
#include <stdexcept>
#include "Parser.h"
namespace prs
{
    bool DescParser::next_line()
    {
        if (ifs_.eof())
            return false;
        std::getline(ifs_, current_line_);
        return true;
    }

    DescParser::DescParser(const std::string file_name) : file_name_(file_name)
    {
        ifs_.open(file_name_);
        if (!ifs_.is_open())
        {
            std::ostringstream ss;
            ss << file_name_ << " is incaccessible\n";
            throw std::invalid_argument(ss.str());
        }
    }

    DescParser::~DescParser()
    {
    }
} // namespace prs