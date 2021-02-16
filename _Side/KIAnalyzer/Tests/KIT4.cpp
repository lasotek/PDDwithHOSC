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
    HOSC::KICluster Cluster({2, 3});
    Cluster.AddEdge({1, 2});
    Cluster.AddEdge({2, 3});
    Cluster.AddEdge({3, 1});
    auto start = std::chrono::steady_clock::now();
    Cluster.Solve();
    auto end = std::chrono::steady_clock::now();
    std::cout << Cluster << std::endl;

    auto elapsed_time = end - start;
    auto i_mills = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time);
    std::cout << "duration time " << i_mills.count() << " microseconds" << std::endl;
}
