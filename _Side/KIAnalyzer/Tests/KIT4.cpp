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

void KirchhoffInexTesting4()
{
    std::cout << "______________KirchhoffInexTesting4_________________\n";
    HOSC::KICluster Cluster({24});
    Cluster.AddEdge({1, 2});
    Cluster.AddEdge({1, 3});
    Cluster.AddEdge({2, 3});
    Cluster.AddEdge({1, 4});
    Cluster.AddEdge({2, 4});
    Cluster.AddEdge({3, 4});
    Cluster.AddEdge({2, 5});
    Cluster.AddEdge({3, 5});
    Cluster.AddEdge({3, 6});
    Cluster.AddEdge({5, 6});
    Cluster.AddEdge({5, 7});
    Cluster.AddEdge({6, 7});
    Cluster.AddEdge({7, 8});
    Cluster.AddEdge({9, 8});
    Cluster.AddEdge({9, 7});
    Cluster.AddEdge({10, 9});
    Cluster.AddEdge({10, 7});
    Cluster.AddEdge({11, 8});
    Cluster.AddEdge({11, 10});
    Cluster.AddEdge({12, 11});
    Cluster.AddEdge({13, 12});
    Cluster.AddEdge({14, 13});
    Cluster.AddEdge({15, 13});
    Cluster.AddEdge({15, 14});
    Cluster.AddEdge({16, 14});
    Cluster.AddEdge({17, 15});
    Cluster.AddEdge({17, 16});
    Cluster.AddEdge({18, 6});
    Cluster.AddEdge({18, 12});
    Cluster.AddEdge({18, 15});
    Cluster.AddEdge({18, 17});
    Cluster.AddEdge({19, 17});
    Cluster.AddEdge({19, 18});
    Cluster.AddEdge({20, 6});
    Cluster.AddEdge({20, 12});
    Cluster.AddEdge({20, 19});
    Cluster.AddEdge({25, 19});
    Cluster.AddEdge({25, 20});
    Cluster.AddEdge({24, 19});
    Cluster.AddEdge({24, 25});
    Cluster.AddEdge({21, 20});
    Cluster.AddEdge({21, 25});
    Cluster.AddEdge({22, 21});
    Cluster.AddEdge({22, 25});
    Cluster.AddEdge({23, 22});
    Cluster.AddEdge({24, 23});
    auto start = std::chrono::steady_clock::now();
#ifdef _OLD_SOLVER_
    Cluster.Solve();
#else
    Cluster.Solve2();
#endif
    auto end = std::chrono::steady_clock::now();
    std::cout << Cluster << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;
}
