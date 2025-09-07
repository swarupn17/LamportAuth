cmake_minimum_required(VERSION 3.10)
project(gen_chain)

set(CMAKE_CXX_STANDARD 17)

find_package(CryptoPP REQUIRED)

add_executable(gen_chain gen_chain.cpp)
target_link_libraries(gen_chain CryptoPP::cryptopp)
