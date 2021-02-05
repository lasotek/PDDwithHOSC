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
