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

void KirchhoffInexTesting7()
{
    // HOSC::cluster_ptr cluster0 = std::make_shared<HOSC::KICluster>({20, 24, 25});

    auto main1_claster = HOSC::new_cluster({20, 18, 13});
    auto cluster0 = HOSC::new_cluster({20, 24, 25});
    // HOSC::KICluster cluster0({20, 24, 25});
    cluster0->AddEdge({20, 21});
    cluster0->AddEdge({20, 25});
    cluster0->AddEdge({21, 22});
    cluster0->AddEdge({21, 25});
    cluster0->AddEdge({22, 23});
    cluster0->AddEdge({22, 25});
    cluster0->AddEdge({23, 24});
    cluster0->AddEdge({24, 25});
    main1_claster->insert_cluster(cluster0, {{20, 20}, {24, 24}, {25, 25}});
    // auto start = std::chrono::steady_clock::now();
    // // cluster0->Solve();
    // auto end = std::chrono::steady_clock::now();
    // std::cout << *cluster0 << std::endl;

    // auto elapsed_time = end - start;
    // auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    // std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    auto cluster1 = HOSC::new_cluster({13, 18, 19});
    // HOSC::KICluster Cluster1({13, 18, 19});
    cluster1->AddEdge({13, 14});
    cluster1->AddEdge({13, 15});
    cluster1->AddEdge({14, 15});
    cluster1->AddEdge({14, 16});
    cluster1->AddEdge({15, 17});
    cluster1->AddEdge({15, 18});
    cluster1->AddEdge({16, 17});
    cluster1->AddEdge({17, 18});
    cluster1->AddEdge({17, 19});
    cluster1->AddEdge({18, 19});
    main1_claster->insert_cluster(cluster1, {{13, 13}, {18, 18}, {19, 19}});
    // start = std::chrono::steady_clock::now();
    // // cluster1->Solve();
    // end = std::chrono::steady_clock::now();
    // std::cout << *cluster1 << std::endl;

    // elapsed_time = end - start;
    // i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    // std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    main1_claster->AddEdge({20, 19});
    main1_claster->AddEdge({25, 19});
    main1_claster->AddEdge({24, 19});
    // auto start = std::chrono::steady_clock::now();
    // main1_claster->Solve();
    // auto end = std::chrono::steady_clock::now();
    // std::cout << *main1_claster << std::endl;

    // auto elapsed_time = end - start;
    // auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    // std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    auto main2_claster = HOSC::new_cluster({6, 12});
    auto cluster2 = HOSC::new_cluster({7, 12});
    // HOSC::KICluster Cluster2({7, 12});
    cluster2->AddEdge({7, 8});
    cluster2->AddEdge({7, 9});
    cluster2->AddEdge({7, 10});
    cluster2->AddEdge({8, 9});
    cluster2->AddEdge({8, 11});
    cluster2->AddEdge({9, 10});
    cluster2->AddEdge({10, 11});
    cluster2->AddEdge({11, 12});
    main2_claster->insert_cluster(cluster2, {{7, 7}, {12, 12}});
    // start = std::chrono::steady_clock::now();
    // // cluster2->Solve();
    // end = std::chrono::steady_clock::now();
    // std::cout << *cluster2 << std::endl;

    // elapsed_time = end - start;
    // i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    // std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    auto cluster3 = HOSC::new_cluster({5, 6});
    // HOSC::KICluster Cluster3({5, 6});
    cluster3->AddEdge({1, 2});
    cluster3->AddEdge({1, 3});
    cluster3->AddEdge({1, 4});
    cluster3->AddEdge({2, 3});
    cluster3->AddEdge({2, 4});
    cluster3->AddEdge({2, 5});
    cluster3->AddEdge({3, 4});
    cluster3->AddEdge({3, 5});
    cluster3->AddEdge({3, 6});
    cluster3->AddEdge({5, 6});
    main2_claster->insert_cluster(cluster3, {{5, 5}, {6, 6}});
    // start = std::chrono::steady_clock::now();
    // // cluster3->Solve();
    // end = std::chrono::steady_clock::now();
    // std::cout << *cluster3 << std::endl;

    // elapsed_time = end - start;
    // i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    // std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    main2_claster->AddEdge({5, 7});
    main2_claster->AddEdge({6, 7});
    // start = std::chrono::steady_clock::now();
    // main2_claster->Solve();
    // end = std::chrono::steady_clock::now();
    // std::cout << *main2_claster << std::endl;

    // elapsed_time = end - start;
    // i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    // std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;

    auto main_global_cluster = HOSC::new_cluster({});
    main_global_cluster->insert_cluster(main1_claster, {{20, 20}, {13, 13}, {18, 18}});
    main_global_cluster->insert_cluster(main2_claster, {{6, 6}, {12, 12}});
    main_global_cluster->AddEdge({20, 6});
    main_global_cluster->AddEdge({20, 12});
    main_global_cluster->AddEdge({13, 12});
    main_global_cluster->AddEdge({18, 6});
    main_global_cluster->AddEdge({18, 12});
    auto start = std::chrono::steady_clock::now();
    main_global_cluster->Solve();
    auto end = std::chrono::steady_clock::now();
    std::cout << *main_global_cluster << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " nanoseconds" << std::endl;
}
