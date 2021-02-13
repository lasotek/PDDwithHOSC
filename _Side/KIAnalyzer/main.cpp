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
#include "HOSCs/SingleDeletion.h"
#include "HOSCs/UniqueHOSCCollections.h"
#include "HOSCs/Cluster.h"
#include "HOSCs/KIIndexCol.h"
#include "HOSCs/KISolver.h"
#include "Tests/KITests.h"



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

        KirchhoffInexTesting8();
        KirchhoffInexTesting();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
