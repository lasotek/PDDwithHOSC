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
#include "HOSCs/SingleDeletion.h"
#include "HOSCs/SymetricHOSC.h"
#include "HOSCs/FullHOSC.h"
#include "HOSCs/UniqueHOSCCollections.h"
#include "HOSCs/SignedUniqueHOSCCollection.h"
/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv)
{
    constexpr auto NoNodes = 10;
    auto unique_coll = HOSC::SignedUniqueHOSCCollection();
    // auto unique_coll = HOSC::HOSCUniqueCollection<HOSC::HOSC>();
    auto del_set1 = HOSC::HOSC::initial_del_set{{{4, 1}, {3, 2}}, {{7, 6}, {4, 1}}, {{9, 3}, {5, 1}}, {{10, 1}, {6, 0}}};
    auto h1 = std::make_shared<HOSC::HOSC>(-1, del_set1, NoNodes);
    assert(h1->is_valid());
    std::cout << h1->String() << std::endl;
    std::cout << "Hash is: " << h1->hash() << std::endl;
    int sgn1 = 1;
    std::tie(sgn1, h1) = unique_coll.sgn_insert_unique(h1); //sufficient to be non-stable
    auto del_set2 = HOSC::HOSC::initial_del_set{{{7, 0}, {7, 0}}, {{8, 0}, {8, 0}}, {{9, 0}, {9, 0}}, {{10, 0}, {10, 0}}};
    auto h2 = std::make_shared<HOSC::HOSC>(1, del_set2, NoNodes);
    assert(h2->is_valid());
    std::cout << h2->String() << std::endl;
    std::cout << "Hash is: " << h2->hash() << std::endl;
    int sgn2 = 1;
    std::tie(sgn2, h2) = unique_coll.sgn_insert_unique(h2);
    auto nodes_to_rem = {6, 7, 8, 9, 10};
    auto h3 = HOSC_big_dot(h1, h2, nodes_to_rem);
    std::cout << "\th1 * h2 is " << h3->String() << std::endl;
    std::cout << "Hash is: " << h3->hash() << std::endl;
    int sgn3 = 1;
    std::tie(sgn3, h3) = unique_coll.sgn_insert_unique(h3);
    auto h4 = std::make_shared<HOSC::HOSC>(*h3);
    std::cout << h4->String() << std::endl;
    std::cout << "Hash is: " << h4->hash() << std::endl;
    int sgn4 = 1;
    std::tie(sgn4, h4) = unique_coll.sgn_insert_unique(h4);
    auto del_set11 = HOSC::HOSC::initial_del_set{{{3, 2}, {3, 2}}, {{6, 0}, {4, 1}}, {{5, 4}, {5, 1}}, {{8, 2}, {6, 0}}};
    auto h11 = std::make_shared<HOSC::HOSC>(-1, del_set11, NoNodes);
    assert(h11->is_valid());
    std::cout << h11->String() << std::endl;
    std::cout << "Hash is: " << h11->hash() << std::endl;
    int sgn11 = 1;
    std::tie(sgn11, h11) = unique_coll.sgn_insert_unique(h11);
    auto h12 = HOSC_big_dot(h11, h2, nodes_to_rem);
    std::cout << "\th11 * h2 is " << h12->String() << std::endl;
    std::cout << "Hash is: " << h12->hash() << std::endl;
    int sgn12 = 1;
    std::tie(sgn12, h12) = unique_coll.sgn_insert_unique(h12);

    auto del_set_f1 = HOSC::HOSC::initial_del_set{{{7, 0}, {6, 7}}};
    auto hf1 = std::make_shared<HOSC::HOSC>(1, del_set_f1, 7);
    std::cout << hf1->String() << std::endl;
    std::cout << "Hash is: " << hf1->hash() << std::endl;
    int sgnf1 = 1;
    std::tie(sgnf1, hf1) = unique_coll.sgn_insert_unique(hf1);
    auto del_set_f2 = HOSC::HOSC::initial_del_set{{{1, -0}, {2, -1}}, {{3, -2}, {4, -3}}, {{5, -4}, {6, -5}}};
    auto hf2 = std::make_shared<HOSC::HOSC>(1, del_set_f2, 7);
    std::cout << hf2->String() << std::endl;
    std::cout << "Hash is: " << hf2->hash() << std::endl;
    int sgnf2 = 1;
    std::tie(sgnf2, hf2) = unique_coll.sgn_insert_unique(hf2);
    auto hf12 = HOSC_big_dot(hf1, hf2, {7});
    std::cout << "\th1f1 * hf2 rem 7 is " << hf12->String() << std::endl;
    std::cout << hf12->String() << std::endl;
    std::cout << "Hash is: " << hf12->hash() << std::endl;
    int sgnf12 = 1;
    std::tie(sgnf12, hf12) = unique_coll.sgn_insert_unique(hf12);

    auto del_set_f3 = HOSC::HOSC::initial_del_set{{{2, 0}, {2, 0}}, {{5, 4}, {5, 4}}, {{6, 4}, {6, 4}}};
    auto hf3 = std::make_shared<HOSC::HOSC>(1, del_set_f3, 6);
    std::cout << hf3->String() << std::endl;
    std::cout << "Hash is: " << hf3->hash() << std::endl;
    int sgnf3 = 1;
    std::tie(sgnf3, hf3) = unique_coll.sgn_insert_unique(hf3);
    auto hf123 = HOSC_big_dot(hf12, hf3, {1, 2, 3, 4, 5, 6});
    std::cout << "\th1f1 * hf2 rem 1,2,3,4,5,6 is " << hf123->String() << std::endl;
    std::cout << hf123->String() << std::endl;
    std::cout << "Hash is: " << hf123->hash() << std::endl;
    int sgnf123 = 1;
    std::tie(sgnf123, hf123) = unique_coll.sgn_insert_unique(hf123);

    for (auto uHOSC : unique_coll)
    {
        std::cout << uHOSC->String() << std::endl;
        std::cout << "Hash is: " << uHOSC->hash() << std::endl;
    }
}
