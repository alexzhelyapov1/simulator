cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(SIMULATOR_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(INSTRUCTIONS_MAP_ROOT_DIR "${SIMULATOR_ROOT_DIR}/instructions_map")
set(MACHINE_ROOT_DIR "${SIMULATOR_ROOT_DIR}/machine")
set(LOADER_ROOT_DIR "${SIMULATOR_ROOT_DIR}/loader")


add_subdirectory(${INSTRUCTIONS_MAP_ROOT_DIR})
add_subdirectory(${MACHINE_ROOT_DIR})
add_subdirectory(${LOADER_ROOT_DIR})



add_custom_target(clang-force-format
    COMMAND find "${INSTRUCTIONS_MAP_ROOT_DIR}" "${MACHINE_ROOT_DIR}" "${LOADER_ROOT_DIR}"
                -name "*.cpp" -exec clang-format -i {} +
)