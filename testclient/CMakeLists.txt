cmake_minimum_required(VERSION 3.17)
project(testclient)

set(CMAKE_CXX_STANDARD 20)

add_executable(testclient SimpleClient.cpp  net_client.h net_common.h net_connection.h net_message.h  net_tsqueue.h olc_net.h md5.h md5.cpp FileWatcher.h)
set(BOOST_ROOT "/opt/homebrew/Cellar/boost/1.75.0_1")
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@1.1")
find_package(OpenSSL REQUIRED)
target_link_libraries(testclient OpenSSL::SSL)
find_package(Boost 1.75.0)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(testclient ${Boost_LIBRARIES})
