cmake_minimum_required(VERSION 3.17)
project(TestingBoostLib)

set(CMAKE_CXX_STANDARD 20)

add_executable(TestingBoostLib main.cpp FileWatcher.cpp FileWatcher.h User.cpp User.h)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(TestingBoostLib ${Boost_LIBRARIES})