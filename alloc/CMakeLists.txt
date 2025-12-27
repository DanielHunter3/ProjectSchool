cmake_minimum_required(VERSION 3.15)
project(alloc_comparison CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Общие флаги компиляции
set(COMMON_FLAGS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(COMMON_FLAGS -Wall -Wextra -Wpedantic -Werror -O3)
elseif(MSVC)
    set(COMMON_FLAGS /W4 /WX)
endif()

# Исходные файлы
set(SINGLE_SRC src/alloc_singlethread.cpp)
set(MULTI_SRC src/alloc_multithread.cpp)

# 1. Системный malloc (по умолчанию)
add_executable(single_system ${SINGLE_SRC})
target_include_directories(single_system PRIVATE include)
target_compile_options(single_system PRIVATE ${COMMON_FLAGS})

add_executable(multi_system ${MULTI_SRC})
target_include_directories(multi_system PRIVATE include)
target_compile_options(multi_system PRIVATE ${COMMON_FLAGS})

# 2. С jemalloc
add_executable(single_jemalloc ${SINGLE_SRC})
add_executable(multi_jemalloc ${MULTI_SRC})

# Ищем jemalloc
find_package(PkgConfig)
pkg_check_modules(JEMALLOC jemalloc)
if(JEMALLOC_FOUND)
    target_include_directories(single_jemalloc PRIVATE include ${JEMALLOC_INCLUDE_DIRS})
    target_link_libraries(single_jemalloc ${JEMALLOC_LIBRARIES})
    target_compile_options(single_jemalloc PRIVATE ${COMMON_FLAGS})
    
    target_include_directories(multi_jemalloc PRIVATE include ${JEMALLOC_INCLUDE_DIRS})
    target_link_libraries(multi_jemalloc ${JEMALLOC_LIBRARIES})
    target_compile_options(multi_jemalloc PRIVATE ${COMMON_FLAGS})
else()
    message(FATAL_ERROR "jemalloc не найден")
endif()

# 3. С tcmalloc
add_executable(single_tcmalloc ${SINGLE_SRC})
add_executable(multi_tcmalloc ${MULTI_SRC})

# Ищем tcmalloc
pkg_check_modules(TCMALLOC libtcmalloc_minimal)
if(TCMALLOC_FOUND)
    target_include_directories(single_tcmalloc PRIVATE include ${TCMALLOC_INCLUDE_DIRS})
    target_link_libraries(single_tcmalloc ${TCMALLOC_LIBRARIES})
    target_compile_options(single_tcmalloc PRIVATE ${COMMON_FLAGS})
    
    target_include_directories(multi_tcmalloc PRIVATE include ${TCMALLOC_INCLUDE_DIRS})
    target_link_libraries(multi_tcmalloc ${TCMALLOC_LIBRARIES})
    target_compile_options(multi_tcmalloc PRIVATE ${COMMON_FLAGS})
else()
    message(FATAL_ERROR "tcmalloc не найден")
endif()

# 4. С mimalloc
add_executable(single_mimalloc ${SINGLE_SRC})
add_executable(multi_mimalloc ${MULTI_SRC})

# Ищем mimalloc
find_library(MIMALLOC_LIB mimalloc)
if(MIMALLOC_LIB)
    target_include_directories(single_mimalloc PRIVATE include)
    target_link_libraries(single_mimalloc ${MIMALLOC_LIB})
    target_compile_options(single_mimalloc PRIVATE ${COMMON_FLAGS})
    
    target_include_directories(multi_mimalloc PRIVATE include)
    target_link_libraries(multi_mimalloc ${MIMALLOC_LIB})
    target_compile_options(multi_mimalloc PRIVATE ${COMMON_FLAGS})
else()
    message(FATAL_ERROR "mimalloc не найден")
endif()

# Создаем кастомные цели для удобства
add_custom_target(all_single
    DEPENDS
        single_system
        single_jemalloc
        single_tcmalloc
        single_mimalloc
)

add_custom_target(all_multi
    DEPENDS
        multi_system
        multi_jemalloc
        multi_tcmalloc
        multi_mimalloc
)

# Мета-цель для сборки всего
add_custom_target(all_allocators
    DEPENDS all_single all_multi
    COMMENT "Building all allocator variants"
)

# Цель по умолчанию
add_custom_target(default_all DEPENDS all_allocators)