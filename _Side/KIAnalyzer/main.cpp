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

void KirchhoffInexTesting()
{
    HOSC::KISolver<long long> Solver;
    Solver.AddEdge({1, 2});
    Solver.AddEdge({1, 3});
    Solver.AddEdge({1, 4});
    Solver.AddEdge({2, 3});
    Solver.AddEdge({2, 4});
    Solver.AddEdge({3, 4});
    Solver.AddEdge({3, 5});
    Solver.AddEdge({3, 6});
    Solver.AddEdge({5, 6});
    Solver.AddEdge({5, 7});
    Solver.AddEdge({6, 7});
    Solver.AddEdge({7, 8});
    Solver.AddEdge({7, 9});
    Solver.AddEdge({7, 10});
    Solver.AddEdge({8, 9});
    Solver.AddEdge({8, 11});
    Solver.AddEdge({9, 10});
    Solver.AddEdge({10, 11});
    Solver.AddEdge({11, 12});
    Solver.AddEdge({6, 18});
    Solver.AddEdge({12, 13});
    Solver.AddEdge({12, 18});
    Solver.AddEdge({13, 14});
    Solver.AddEdge({13, 15});
    Solver.AddEdge({14, 15});
    Solver.AddEdge({14, 16});
    Solver.AddEdge({15, 17});
    Solver.AddEdge({15, 18});
    Solver.AddEdge({16, 17});
    Solver.AddEdge({17, 18});
    Solver.AddEdge({17, 19});
    Solver.AddEdge({18, 19});
    Solver.AddEdge({6, 20});
    Solver.AddEdge({12, 20});
    Solver.AddEdge({19, 20});
    Solver.AddEdge({19, 24});
    Solver.AddEdge({19, 25});
    Solver.AddEdge({20, 21});
    Solver.AddEdge({20, 25});
    Solver.AddEdge({21, 22});
    Solver.AddEdge({21, 25});
    Solver.AddEdge({22, 23});
    Solver.AddEdge({22, 25});
    Solver.AddEdge({23, 24});
    Solver.AddEdge({24, 25});
    auto start = std::chrono::steady_clock::now();
    Solver.Solve();
    auto end = std::chrono::steady_clock::now();
    // std::time_t end_time = std::chrono::system_clock::to_time_t
    if (Solver.is_ready_and_connected())
    {
        std::cout << "Numerator = " << Solver.numerator() << std::endl;
        std::cout << "Denominator = " << Solver.denominator() << std::endl;
        std::cout << "KIndex = " << Solver.KIIndex() << std::endl;
    }
    else
    {
        std::cout << "The graph is unsolved or disconneced\n";
    }

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;
}
void KirchhoffInexTesting2()
{
    HOSC::KISolver<long long> Solver;
    Solver.AddEdge({1, 2});
    Solver.AddEdge({2, 3});
    Solver.AddEdge({3, 1});
    Solver.AddEdge({2, 5});
    Solver.AddEdge({3, 6});
    Solver.AddEdge({4, 5});
    Solver.AddEdge({5, 6});
    Solver.AddEdge({6, 4});
    // Solver.AddEdge({8, 9});//
    // Solver.AddEdge({9, 10});
    // Solver.AddEdge({10, 11});
    // Solver.AddEdge({11, 12});
    // Solver.AddEdge({12, 13});
    // Solver.AddEdge({13, 14});
    // Solver.AddEdge({14, 15});
    // Solver.AddEdge({15, 16});
    // Solver.AddEdge({16, 17});
    // Solver.AddEdge({17, 18});
    // Solver.AddEdge({18, 19});
    // Solver.AddEdge({19, 20});
    // Solver.AddEdge({20, 21});
    // Solver.AddEdge({21, 22});
    // Solver.AddEdge({22, 23});
    // Solver.AddEdge({23, 24});
    // Solver.AddEdge({24, 25});
    // Solver.AddEdge({25, 26});
    // Solver.AddEdge({26, 27});
    // Solver.AddEdge({27, 28});
    // Solver.AddEdge({28, 29});
    // Solver.AddEdge({29, 30});
    // Solver.AddEdge({30, 31});
    // Solver.AddEdge({31, 32});
    // Solver.AddEdge({15, 18});
    // Solver.AddEdge({16, 17});
    // Solver.AddEdge({17, 18});
    // Solver.AddEdge({17, 19});
    // Solver.AddEdge({18, 19});
    // Solver.AddEdge({6, 20});
    // Solver.AddEdge({12, 20});
    // Solver.AddEdge({19, 20});
    // Solver.AddEdge({19, 24});
    // Solver.AddEdge({19, 25});
    // Solver.AddEdge({20, 21});
    // Solver.AddEdge({20, 25});
    // Solver.AddEdge({21, 22});
    // Solver.AddEdge({21, 25});
    // Solver.AddEdge({22, 23});
    // Solver.AddEdge({22, 25});
    // Solver.AddEdge({23, 24});
    // Solver.AddEdge({24, 25});
    auto start = std::chrono::steady_clock::now();
    Solver.Solve();
    auto end = std::chrono::steady_clock::now();
    // std::time_t end_time = std::chrono::system_clock::to_time_t
    if (Solver.is_ready_and_connected())
    {
        std::cout << "Numerator = " << Solver.numerator() << std::endl;
        std::cout << "Denominator = " << Solver.denominator() << std::endl;
        std::cout << "KIndex = " << Solver.KIIndex() << std::endl;
    }
    else
    {
        std::cout << "The graph is unsolved or disconneced\n";
    }

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;
}
void KirchhoffInexTesting3()
{
    HOSC::KISolver<long long> Solver;
    Solver.AddEdge({1, 2});
    Solver.AddEdge({2, 3});
    Solver.AddEdge({3, 4});
    Solver.AddEdge({4, 1});
    Solver.AddEdge({2, 4});
    Solver.AddEdge({1, 3});
    // Solver.AddEdge({7, 8});
    // Solver.AddEdge({8, 9});//
    // Solver.AddEdge({9, 10});
    // Solver.AddEdge({10, 11});
    // Solver.AddEdge({11, 12});
    // Solver.AddEdge({12, 13});
    // Solver.AddEdge({13, 14});
    // Solver.AddEdge({14, 15});
    // Solver.AddEdge({15, 16});
    // Solver.AddEdge({16, 17});
    // Solver.AddEdge({17, 18});
    // Solver.AddEdge({18, 19});
    // Solver.AddEdge({19, 20});
    // Solver.AddEdge({20, 21});
    // Solver.AddEdge({21, 22});
    // Solver.AddEdge({22, 23});
    // Solver.AddEdge({23, 24});
    // Solver.AddEdge({24, 25});
    // Solver.AddEdge({25, 26});
    // Solver.AddEdge({26, 27});
    // Solver.AddEdge({27, 28});
    // Solver.AddEdge({28, 29});
    // Solver.AddEdge({29, 30});
    // Solver.AddEdge({30, 31});
    // Solver.AddEdge({31, 32});
    // Solver.AddEdge({15, 18});
    // Solver.AddEdge({16, 17});
    // Solver.AddEdge({17, 18});
    // Solver.AddEdge({17, 19});
    // Solver.AddEdge({18, 19});
    // Solver.AddEdge({6, 20});
    // Solver.AddEdge({12, 20});
    // Solver.AddEdge({19, 20});
    // Solver.AddEdge({19, 24});
    // Solver.AddEdge({19, 25});
    // Solver.AddEdge({20, 21});
    // Solver.AddEdge({20, 25});
    // Solver.AddEdge({21, 22});
    // Solver.AddEdge({21, 25});
    // Solver.AddEdge({22, 23});
    // Solver.AddEdge({22, 25});
    // Solver.AddEdge({23, 24});
    // Solver.AddEdge({24, 25});
    auto start = std::chrono::steady_clock::now();
    Solver.Solve();
    auto end = std::chrono::steady_clock::now();
    // std::time_t end_time = std::chrono::system_clock::to_time_t
    if (Solver.is_ready_and_connected())
    {
        std::cout << "Numerator = " << Solver.numerator() << std::endl;
        std::cout << "Denominator = " << Solver.denominator() << std::endl;
        std::cout << "KIndex = " << Solver.KIIndex() << std::endl;
    }
    else
    {
        std::cout << "The graph is unsolved or disconneced\n";
    }

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;
}
void KirchhoffInexTesting4()
{
    HOSC::KICluster Cluster({2, 3});
    Cluster.AddEdge({1, 2});
    Cluster.AddEdge({2, 3});
    Cluster.AddEdge({3, 1});
    auto start = std::chrono::steady_clock::now();
    Cluster.Solve();
    auto end = std::chrono::steady_clock::now();
    std::cout << Cluster << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;
}

void KirchhoffInexTesting5()
{
    HOSC::KICluster Cluster({1, 4});
    Cluster.AddEdge({1, 2});
    Cluster.AddEdge({2, 3});
    Cluster.AddEdge({3, 4});
    auto start = std::chrono::steady_clock::now();
    Cluster.Solve();
    auto end = std::chrono::steady_clock::now();
    std::cout << Cluster << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster2({1, 8});
    Cluster2.AddEdge({1, 2});
    Cluster2.AddEdge({2, 3});
    Cluster2.AddEdge({3, 4});
    Cluster2.AddEdge({4, 5});
    Cluster2.AddEdge({5, 6});
    Cluster2.AddEdge({6, 7});
    Cluster2.AddEdge({7, 8});
    start = std::chrono::steady_clock::now();
    Cluster2.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster2 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster3({1, 16});
    Cluster3.AddEdge({1, 2});
    Cluster3.AddEdge({2, 3});
    Cluster3.AddEdge({3, 4});
    Cluster3.AddEdge({4, 5});
    Cluster3.AddEdge({5, 6});
    Cluster3.AddEdge({6, 7});
    Cluster3.AddEdge({7, 8});
    Cluster3.AddEdge({8, 9});
    Cluster3.AddEdge({9, 10});
    Cluster3.AddEdge({10, 11});
    Cluster3.AddEdge({11, 12});
    Cluster3.AddEdge({12, 13});
    Cluster3.AddEdge({13, 14});
    Cluster3.AddEdge({14, 15});
    Cluster3.AddEdge({15, 16});
    start = std::chrono::steady_clock::now();
    Cluster3.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster3 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;
}

void KirchhoffInexTesting6()
{
    HOSC::KICluster Cluster0({20, 24, 25});
    Cluster0.AddEdge({20, 21});
    Cluster0.AddEdge({20, 25});
    Cluster0.AddEdge({21, 22});
    Cluster0.AddEdge({21, 25});
    Cluster0.AddEdge({22, 23});
    Cluster0.AddEdge({22, 25});
    Cluster0.AddEdge({23, 24});
    Cluster0.AddEdge({24, 25});
    auto start = std::chrono::steady_clock::now();
    Cluster0.Solve();
    auto end = std::chrono::steady_clock::now();
    std::cout << Cluster0 << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster1({13, 18, 19});
    start = std::chrono::steady_clock::now();
    Cluster1.AddEdge({13, 14});
    Cluster1.AddEdge({13, 15});
    Cluster1.AddEdge({14, 15});
    Cluster1.AddEdge({14, 16});
    Cluster1.AddEdge({15, 17});
    Cluster1.AddEdge({15, 18});
    Cluster1.AddEdge({16, 17});
    Cluster1.AddEdge({17, 18});
    Cluster1.AddEdge({17, 19});
    Cluster1.AddEdge({18, 19});
    Cluster1.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster1 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster2({7, 12});
    Cluster2.AddEdge({7, 8});
    Cluster2.AddEdge({7, 9});
    Cluster2.AddEdge({7, 10});
    Cluster2.AddEdge({8, 9});
    Cluster2.AddEdge({8, 11});
    Cluster2.AddEdge({9, 10});
    Cluster2.AddEdge({10, 11});
    Cluster2.AddEdge({11, 12});
    start = std::chrono::steady_clock::now();
    Cluster2.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster2 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster3({5, 6});
    Cluster3.AddEdge({1, 2});
    Cluster3.AddEdge({1, 3});
    Cluster3.AddEdge({1, 4});
    Cluster3.AddEdge({2, 3});
    Cluster3.AddEdge({2, 4});
    Cluster3.AddEdge({3, 4});
    Cluster3.AddEdge({3, 5});
    Cluster3.AddEdge({3, 6});
    Cluster3.AddEdge({5, 6});
    start = std::chrono::steady_clock::now();
    Cluster3.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster3 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster01({13, 18, 19, 20});
    Cluster01.AddEdge({13, 14});
    Cluster01.AddEdge({13, 15});
    Cluster01.AddEdge({14, 15});
    Cluster01.AddEdge({14, 16});
    Cluster01.AddEdge({15, 17});
    Cluster01.AddEdge({15, 18});
    Cluster01.AddEdge({16, 17});
    Cluster01.AddEdge({17, 18});
    Cluster01.AddEdge({17, 19});
    Cluster01.AddEdge({18, 19});
    Cluster01.AddEdge({19, 20});
    Cluster01.AddEdge({19, 24});
    Cluster01.AddEdge({19, 25});
    Cluster01.AddEdge({20, 21});
    Cluster01.AddEdge({20, 25});
    Cluster01.AddEdge({21, 22});
    Cluster01.AddEdge({21, 25});
    Cluster01.AddEdge({22, 23});
    Cluster01.AddEdge({22, 25});
    Cluster01.AddEdge({23, 24});
    Cluster01.AddEdge({24, 25});
    start = std::chrono::steady_clock::now();
    Cluster01.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster01 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    HOSC::KICluster Cluster23({5, 6});
    Cluster23.AddEdge({1, 2});
    Cluster23.AddEdge({1, 3});
    Cluster23.AddEdge({1, 4});
    Cluster23.AddEdge({2, 3});
    Cluster23.AddEdge({2, 4});
    Cluster23.AddEdge({3, 4});
    Cluster23.AddEdge({3, 5});
    Cluster23.AddEdge({3, 6});
    Cluster23.AddEdge({5, 6});
    Cluster23.AddEdge({5, 7});
    Cluster23.AddEdge({6, 7});
    Cluster23.AddEdge({7, 8});
    Cluster23.AddEdge({7, 9});
    Cluster23.AddEdge({7, 10});
    Cluster23.AddEdge({8, 9});
    Cluster23.AddEdge({8, 11});
    Cluster23.AddEdge({9, 10});
    Cluster23.AddEdge({10, 11});
    Cluster23.AddEdge({11, 12});
    start = std::chrono::steady_clock::now();
    Cluster3.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster3 << std::endl;
}
/**
 * @brief Main entry for testing
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    try
    {

        KirchhoffInexTesting6();
        KirchhoffInexTesting();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
