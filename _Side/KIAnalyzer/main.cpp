/**
 * @file main.cpp
 * @author Sławomir Lasota  (lasotek@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <iostream>
#include <cassert>
#include <memory>
#include <thread>
#include <chrono>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <optional>
#include <filesystem>
#include <string.h>
#include <fstream>
#include <bits/stdc++.h>
#include "HOSCs/SingleDeletion.h"
#include "HOSCs/UniqueHOSCCollections.h"
#include "HOSCs/Cluster.h"
#include "HOSCs/KIIndexCol.h"
#include "HOSCs/KISolver.h"
#include "Tests/KITests.h"
#include "HOSCs/Parser/Parser.h"

void info()
{
    std::ostringstream ss;
    ss << "The calling form is:\n";
    ss << "\t KIAnalyzer -i <input_text_file> {-o <output_text_file>}\n";
    ss << "In case of lack of output file, results are put to default stream";
    ss << "Input file must exists, and have legal form ";
    throw std::invalid_argument(ss.str());
}

std::pair<std::optional<std::string>, std::optional<std::string>> det_inp_and_out_file_name(int argc, char **argv)
{
    if (argc == 3)
    {
        std::string par(argv[1]);
        if (strcmp(argv[2], "-i") != 0)
            info();
        std::string in_file_name(argv[3]);
        if (!std::filesystem::exists(in_file_name))
            info();
        return std::make_pair<std::optional<std::string>, std::optional<std::string>>(in_file_name, {});
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-o") == 0)
        {
            std::string in_file_name(argv[2]);
            if (!std::filesystem::exists(in_file_name))
                info();
            std::string out_file_name(argv[4]);
            return std::make_pair<std::optional<std::string>, std::optional<std::string>>(in_file_name, out_file_name);
        }
        else if (strcmp(argv[1], "-o") == 0 && strcmp(argv[3], "-i") == 0)
        {
            std::string in_file_name(argv[4]);
            if (!std::filesystem::exists(in_file_name))
                info();
            std::string out_file_name(argv[2]);
            return std::make_pair<std::optional<std::string>, std::optional<std::string>>(in_file_name, out_file_name);
        }
        else
            info();
    }
    else
        info();
    return std::make_pair<std::optional<std::string>, std::optional<std::string>>({}, {});
}
/**
 * @brief Main entry for testing
 * 
 * @param argc the number of arguments
 * @param argv arguments
 * @return int error return
 */
int main(int argc, char **argv)
{
    try
    {

        // auto [in_file_name, out_file_name] = det_inp_and_out_file_name(argc, argv);
        // std::ofstream out_f;
        // if(out_file_name)
        // {
        //     out_f.open(out_file_name.value());
        //     std::ios_base::sync_with_stdio(false);
        //     std::cout.rdbuf(out_f.rdbuf());
        // }
        // prs::DescParser parser(in_file_name.value());
        // std::ostream out_st (std::cout);
        // if(in_file_name)
            // out_st = ofstream.
        auto start = std::chrono::steady_clock::now();
        auto end = std::chrono::steady_clock::now();
        // std::time_t end_time = std::chrono::system_clock::to_time_t

        auto elapsed_time = end - start;
        auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
        std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

        KirchhoffInexTesting();
        KirchhoffInexTesting4();
        KirchhoffInexTesting7();
        KirchhoffInexTesting12();
        KirchhoffInexTesting10();
        KirchhoffInexTesting8();
        KirchhoffInexTesting9();
        KirchhoffInexTesting11();
        // KirchhoffInexTesting6();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
