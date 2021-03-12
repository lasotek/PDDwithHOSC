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

void KirchhoffInexTesting12()
{
    std::cout << "______________KirchhoffInexTesting12_________________\n";

    HOSC::KICluster Cluster1({13, 18, 19});
    auto start = std::chrono::steady_clock::now();
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
#ifdef _OLD_SOLVER_
    Cluster1.Solve();
#else
    Cluster1.Solve2();
#endif
    auto end = std::chrono::steady_clock::now();
    std::cout << Cluster1 << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster0({20, 24, 25});
    Cluster0.AddEdge({20, 21});
    Cluster0.AddEdge({20, 25});
    Cluster0.AddEdge({21, 22});
    Cluster0.AddEdge({21, 25});
    Cluster0.AddEdge({22, 23});
    Cluster0.AddEdge({22, 25});
    Cluster0.AddEdge({23, 24});
    Cluster0.AddEdge({24, 25});
    start = std::chrono::steady_clock::now();
#ifdef _OLD_SOLVER_
    Cluster0.Solve();
#else
    Cluster0.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster0 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster1e({13, 18, 20, 24, 25});
    start = std::chrono::steady_clock::now();
    Cluster1e.AddEdge({13, 14});
    Cluster1e.AddEdge({13, 15});
    Cluster1e.AddEdge({14, 15});
    Cluster1e.AddEdge({14, 16});
    Cluster1e.AddEdge({15, 17});
    Cluster1e.AddEdge({15, 18});
    Cluster1e.AddEdge({16, 17});
    Cluster1e.AddEdge({17, 18});
    Cluster1e.AddEdge({17, 19});
    Cluster1e.AddEdge({18, 19});
    Cluster1e.AddEdge({20, 19});
    Cluster1e.AddEdge({24, 19});
    Cluster1e.AddEdge({25, 19});
#ifdef _OLD_SOLVER_
    Cluster1e.Solve();
#else
    Cluster1e.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster1e << std::endl;

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
#ifdef _OLD_SOLVER_
    Cluster01.Solve();
#else
    Cluster01.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster01 << std::endl;

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
#ifdef _OLD_SOLVER_
    Cluster2.Solve();
#else
    Cluster2.Solve2();
#endif
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
    Cluster3.AddEdge({2, 5});
    Cluster3.AddEdge({3, 4});
    Cluster3.AddEdge({3, 5});
    Cluster3.AddEdge({3, 6});
    Cluster3.AddEdge({5, 6});
    start = std::chrono::steady_clock::now();
#ifdef _OLD_SOLVER_
    Cluster3.Solve();
#else
    Cluster3.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster3 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster3e({6, 7});
    Cluster3e.AddEdge({1, 2});
    Cluster3e.AddEdge({1, 3});
    Cluster3e.AddEdge({1, 4});
    Cluster3e.AddEdge({2, 3});
    Cluster3e.AddEdge({2, 4});
    Cluster3e.AddEdge({2, 5});
    Cluster3e.AddEdge({3, 4});
    Cluster3e.AddEdge({3, 5});
    Cluster3e.AddEdge({3, 6});
    Cluster3e.AddEdge({5, 6});
    Cluster3e.AddEdge({5, 7});
    Cluster3e.AddEdge({6, 7});
    start = std::chrono::steady_clock::now();
#ifdef _OLD_SOLVER_
    Cluster3e.Solve();
#else
    Cluster3e.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster3e << std::endl;

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
#ifdef _OLD_SOLVER_
    Cluster23.Solve();
#else
    Cluster23.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster23 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster23e({13,18,20});
    Cluster23e.AddEdge({1, 2});
    Cluster23e.AddEdge({1, 3});
    Cluster23e.AddEdge({1, 4});
    Cluster23e.AddEdge({2, 3});
    Cluster23e.AddEdge({2, 4});
    Cluster23e.AddEdge({3, 4});
    Cluster23e.AddEdge({3, 5});
    Cluster23e.AddEdge({3, 6});
    Cluster23e.AddEdge({5, 6});
    Cluster23e.AddEdge({5, 7});
    Cluster23e.AddEdge({6, 7});
    Cluster23e.AddEdge({6, 18});
    Cluster23e.AddEdge({6, 20});
    Cluster23e.AddEdge({7, 8});
    Cluster23e.AddEdge({7, 9});
    Cluster23e.AddEdge({7, 10});
    Cluster23e.AddEdge({8, 9});
    Cluster23e.AddEdge({8, 11});
    Cluster23e.AddEdge({9, 10});
    Cluster23e.AddEdge({10, 11});
    Cluster23e.AddEdge({11, 12});
    Cluster23e.AddEdge({12, 13});
    Cluster23e.AddEdge({12, 18});
    Cluster23e.AddEdge({12, 20});
    start = std::chrono::steady_clock::now();
#ifdef _OLD_SOLVER_
    Cluster23e.Solve();
#else
    Cluster23e.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster23e << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;

    HOSC::KICluster Cluster0123({20});
    Cluster0123.AddEdge({1, 2});
    Cluster0123.AddEdge({1, 3});
    Cluster0123.AddEdge({1, 4});
    Cluster0123.AddEdge({2, 3});
    Cluster0123.AddEdge({2, 4});
    Cluster0123.AddEdge({2, 5});
    Cluster0123.AddEdge({3, 4});
    Cluster0123.AddEdge({3, 5});
    Cluster0123.AddEdge({3, 6});
    Cluster0123.AddEdge({5, 6});
    Cluster0123.AddEdge({5, 7});
    Cluster0123.AddEdge({6, 7});
    Cluster0123.AddEdge({7, 8});
    Cluster0123.AddEdge({7, 9});
    Cluster0123.AddEdge({7, 10});
    Cluster0123.AddEdge({8, 9});
    Cluster0123.AddEdge({8, 11});
    Cluster0123.AddEdge({9, 10});
    Cluster0123.AddEdge({10, 11});
    Cluster0123.AddEdge({11, 12});
    Cluster0123.AddEdge({6, 18});
    Cluster0123.AddEdge({12, 13});
    Cluster0123.AddEdge({12, 18});
    Cluster0123.AddEdge({13, 14});
    Cluster0123.AddEdge({13, 15});
    Cluster0123.AddEdge({14, 15});
    Cluster0123.AddEdge({14, 16});
    Cluster0123.AddEdge({15, 17});
    Cluster0123.AddEdge({15, 18});
    Cluster0123.AddEdge({16, 17});
    Cluster0123.AddEdge({17, 18});
    Cluster0123.AddEdge({17, 19});
    Cluster0123.AddEdge({18, 19});
    Cluster0123.AddEdge({6, 20});
    Cluster0123.AddEdge({12, 20});
    Cluster0123.AddEdge({19, 20});
    Cluster0123.AddEdge({19, 24});
    Cluster0123.AddEdge({19, 25});
    Cluster0123.AddEdge({20, 21});
    Cluster0123.AddEdge({20, 25});
    Cluster0123.AddEdge({21, 22});
    Cluster0123.AddEdge({21, 25});
    Cluster0123.AddEdge({22, 23});
    Cluster0123.AddEdge({22, 25});
    Cluster0123.AddEdge({23, 24});
    Cluster0123.AddEdge({24, 25});
    start = std::chrono::steady_clock::now();
#ifdef _OLD_SOLVER_
    Cluster0123.Solve();
#else
    Cluster0123.Solve2();
#endif
    end = std::chrono::steady_clock::now();
    std::cout << Cluster0123 << std::endl;

    elapsed_time = end - start;
    i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;
}
