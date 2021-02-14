cmake_minimum_required(VERSION 3.0.0)
project(HOSCAnalyzer VERSION 0.1.0)

set(GCC_CPP_VERSION_FLAGS "-std=c++20 -fcoroutines ")
# -fcoroutines -static
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CPP_VERSION_FLAGS}")

include(CTest)
enable_testing()

add_executable(KIAnalyzer main.cpp 
                            ./HOSCs/SingleDeletion.cpp 
                            ./HOSCs/helpers/helpers.cpp 
                            ./HOSCs/KIIndexCol.cpp
                            ./HOSCs/SingleHOSC.cpp
                            ./HOSCs/Cluster.cpp
                            ./Tests/KIT1.cpp
                            ./Tests/KIT2.cpp
                            ./Tests/KIT3.cpp
                            ./Tests/KIT4.cpp
                            ./Tests/KIT5.cpp
                            ./Tests/KIT6.cpp
                            ./Tests/KIT7.cpp
                            ./Tests/KIT8.cpp
                            ./Tests/KIT9.cpp
                            ./Tests/KIT10.cpp
                            )
                            
    
# add_definitions(-D _DEBUG_TEST)
    
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
