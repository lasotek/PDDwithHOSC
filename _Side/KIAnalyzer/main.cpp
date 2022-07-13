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

const std::string description =
    "Calling of program is:\n"
    "HOSCAnalyzer -i <input_file> -o <output_file>\n";

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
        // KirchhoffIndexTesting10();
        // return 0;
        std::string InputFileName, OutputFileName;
        if (argc != 5)
        {
            std::cout << description;
            return -1;
        }
        std::span arg_list(argv + 1, argc - 1);
        for (auto it = arg_list.begin(); it != arg_list.end(); it++)
        {
            if (strcmp(*it, "-i") == 0)
                InputFileName = *(++it);
            else if (strcmp(*it, "-o") == 0)
                OutputFileName = *(++it);
            else
            {
                std::cout << description;
                return -1;
            }
        }
        if (!std::filesystem::exists(InputFileName))
        {
            std::cout << "Input file " << InputFileName << " does not exist" << std::endl;
            return -1;
        }
        if ((std::filesystem::exists(OutputFileName)) && (!std::filesystem::is_empty(OutputFileName)))
        {
            std::cout << "Output file " << OutputFileName << " already exists" << std::endl;
            std::cout << "Do you want to overwrite it? (y/n)" << std::endl;
            char c;
            std::cin >> c;
            if (toupper(c) != 'Y')
                return -1;
        }
        prs::Parser parser(InputFileName, OutputFileName);
        parser.run();
        // std::ostream &os = parser;

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
        std::cout << "# duration time of nothing is " << i_mills.count() << " nanoseconds" << std::endl;

        // KirchhoffIndexTesting();
        // KirchhoffIndexTesting4();
        // KirchhoffIndexTesting7();
        // KirchhoffIndexTesting12();
        // KirchhoffIndexTesting10();
        // KirchhoffIndexTesting8();
        // KirchhoffIndexTesting9();
        // KirchhoffIndexTesting11();
        // KirchhoffIndexTesting6();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
