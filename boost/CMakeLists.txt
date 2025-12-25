cmake_minimum_required(VERSION 3.15)
project(allocator_benchmark CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Оптимизации
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -pthread")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
endif()

# Boost
find_package(Boost 1.66 REQUIRED COMPONENTS container)

# Включаем директории
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ===== SINGLE THREADED =====

# Vec - single
add_executable(single_vec_std src/vec/single/std.cpp)
target_link_libraries(single_vec_std PRIVATE Boost::headers)

add_executable(single_vec_boost src/vec/single/boost.cpp)
target_link_libraries(single_vec_boost PRIVATE Boost::headers Boost::container)

# Pool - single
add_executable(single_pool_std src/pool/single/std.cpp)
target_link_libraries(single_pool_std PRIVATE Boost::headers)

add_executable(single_pool_boost src/pool/single/boost.cpp)
target_link_libraries(single_pool_boost PRIVATE Boost::headers)

# Map - single
add_executable(single_map_std src/map/single/std.cpp)
target_link_libraries(single_map_std PRIVATE Boost::headers)

add_executable(single_map_boost src/map/single/boost.cpp)
target_link_libraries(single_map_boost PRIVATE Boost::headers)

# ===== MULTI THREADED =====

# Vec - multi
add_executable(multi_vec_std src/vec/multi/std.cpp)
target_link_libraries(multi_vec_std PRIVATE pthread Boost::headers)

add_executable(multi_vec_boost src/vec/multi/boost.cpp)
target_link_libraries(multi_vec_boost PRIVATE pthread Boost::headers Boost::container)

# Pool - multi
add_executable(multi_pool_std src/pool/multi/std.cpp)
target_link_libraries(multi_pool_std PRIVATE pthread Boost::headers)

add_executable(multi_pool_boost src/pool/multi/boost.cpp)
target_link_libraries(multi_pool_boost PRIVATE pthread Boost::headers)

# Map - multi
add_executable(multi_map_std src/map/multi/std.cpp)
target_link_libraries(multi_map_std PRIVATE pthread Boost::headers)

add_executable(multi_map_boost src/map/multi/boost.cpp)
target_link_libraries(multi_map_boost PRIVATE pthread Boost::headers)

# Группировка целей для удобства
add_custom_target(all_single
    DEPENDS 
        single_vec_std single_vec_boost
        single_pool_std single_pool_boost
        single_map_std single_map_boost
)

add_custom_target(all_multi
    DEPENDS 
        multi_vec_std multi_vec_boost
        multi_pool_std multi_pool_boost
        multi_map_std multi_map_boost
)

add_custom_target(all_benchmarks
    DEPENDS all_single all_multi
    COMMENT "Собраны все бенчмарки"
)

# Установка (опционально)
install(TARGETS 
    single_vec_std single_vec_boost
    single_pool_std single_pool_boost
    single_map_std single_map_boost
    multi_vec_std multi_vec_boost
    multi_pool_std multi_pool_boost
    multi_map_std multi_map_boost
    RUNTIME DESTINATION bin
)