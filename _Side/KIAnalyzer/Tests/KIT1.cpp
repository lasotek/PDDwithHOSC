#include "KITests.h"
#include <iostream>
#include <cassert>
#include <memory>
#include <thread>
#include <chrono>
#include <ctime>
#include "../HOSCs/SingleDeletion.h"
#include "../HOSCs/UniqueHOSCCollections.h"
#include "../HOSCs/Cluster.h"
#include "../HOSCs/KIIndexCol.h"
#include "../HOSCs/KISolver.h"

void KirchhoffInexTesting()
{
    HOSC::KISolver<long long> Solver;
    Solver.AddEdge({1, 2});
    Solver.AddEdge({1, 3});
    Solver.AddEdge({1, 4});
    Solver.AddEdge({2, 3});
    Solver.AddEdge({2, 4});
    Solver.AddEdge({2, 5});
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
        std::cout <<"Plain Analysis\n";
        std::cout << "Numerator = " << Solver.numerator() << std::endl;
        std::cout << "Denominator = " << Solver.denominator() << std::endl;
        std::cout << "KIndex = " << Solver.KIIndex() << std::endl;
    }
    else
    {
        std::cout << "The graph is unsolved or disconneced\n";
    }

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;
}
