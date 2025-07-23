cmake_minimum_required(VERSION 3.10)
project(allocator_benchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -march=native")

# Boost (только заголовки)
find_package(Boost REQUIRED COMPONENTS headers)

# Оригинальные тесты (без jemalloc)
add_executable(single_vec src/single_vec.cpp)
target_link_libraries(single_vec PRIVATE Boost::headers)

add_executable(single_pool src/single_pool.cpp)
target_link_libraries(single_pool PRIVATE Boost::headers)

add_executable(multi_vec src/multi_vec.cpp)
target_link_libraries(multi_vec PRIVATE Boost::headers)

add_executable(multi_pool src/multi_pool.cpp)
target_link_libraries(multi_pool PRIVATE Boost::headers)