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
    auto i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;
}
