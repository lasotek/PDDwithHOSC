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
    auto i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

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
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

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
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

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
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster01({13, 18, 20});
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
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster23({6, 12});
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
    Cluster23.Solve();
    end = std::chrono::steady_clock::now();
    std::cout << Cluster23 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;
}
