cmake_minimum_required(VERSION 3.17)
project(TestingBoostLib)

set(CMAKE_CXX_STANDARD 20)



find_package(Boost COMPONENTS  system filesystem)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(TestingBoostLib main.cpp)

target_link_libraries(TestingBoostLib ${Boost_LIBRARIES})

add_executable(TestingBoostLi main.cpp FileWatcher.h FileWatcher.cpp)


