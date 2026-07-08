cmake_minimum_required(VERSION 3.20)

project(socket_helper)

add_library(socket_helper
    socket_helper.hpp
    socket_helper.cpp
)

target_compile_features(socket_helper PUBLIC cxx_std_20)

target_link_libraries(socket_helper PUBLIC
    cpp_helper
)

target_include_directories(socket_helper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(socket_helper_test
    test.cpp
)

target_compile_features(socket_helper_test PUBLIC cxx_std_20)

target_link_libraries(socket_helper_test PUBLIC
    socket_helper
)

