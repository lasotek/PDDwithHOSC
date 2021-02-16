#ifndef __PARSER_H__
#define __PARSER_H__
#include <string>
#include <fstream>

namespace prs
{
    class DescParser
    {
    private:
        std::string file_name_;
        std::ifstream ifs_;
        std::string current_line_;

        bool next_line();
    public:
        DescParser(const std::string file_name);
        ~DescParser();
    };

} // namespace prs

#endif // __PARSER_H__