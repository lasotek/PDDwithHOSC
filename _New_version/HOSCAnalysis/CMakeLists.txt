cmake_minimum_required(VERSION 3.0.0)
project(HOSCAnalyzer VERSION 0.1.0)

set(GCC_CPP_VERSION_FLAGS "-std=c++20 -fcoroutines")
# -fcoroutines -static
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CPP_VERSION_FLAGS}")

include(CTest)
enable_testing()

add_executable(HOSCAnalyzer main.cpp
                            ./HOSCs/SingleDeletion.cpp 
                            # ./HOSCs/SymetricHOSC.cpp 
                            ./HOSCs/FullHOSC.cpp 
                            ./HOSCs/helpers/helpers.cpp 
                            ./HOSCs/SignedUniqueHOSCCollection.cpp
                            ./HOSCs/MultilevelHOSC.cpp
                            ./HOSCs/Parser.cpp
                            ./HOSCs/helpers/NodeMaper.cpp
                            ./HOSCs/HOSCContainer.cpp
                            ./HOSCs/HOSCOperator.cpp
                            # ./HOSCs/KIIndexCol.cpp
                            # ./HOSCs/SingleHOSC.cpp
)

# if (CMAKE_BUILD_TYPE EQUAL "Debug")
# endif (CMAKE_BUILD_TYPE EQUAL "Debug")                            
# if (CMAKE_CPP_FLAGS_DEBUG)
# endif ((CMAKE_C_FLAGS_DEBUG)                            
    
# add_definitions(-D _DEBUG_TEST)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
