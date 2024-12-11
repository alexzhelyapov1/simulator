cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(INSTRUCTIONS_MAP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/instructions_map")
set(MACHINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/machine")
set(SIMULATOR_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/simulator")
set(LOADER_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/loader")
set(MAIN_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/main")
set(BUILD_DIR_GEN "${CMAKE_BINARY_DIR}/generated")
set(CACHE_DIR "${MACHINE_ROOT_DIR}/cache")
set(TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools")


add_subdirectory(${INSTRUCTIONS_MAP_ROOT_DIR})
add_subdirectory(${LOADER_ROOT_DIR})

set(SIMULATOR_SRC
${MACHINE_ROOT_DIR}/machine.cpp
${MACHINE_ROOT_DIR}/hart.cpp
${MACHINE_ROOT_DIR}/mem.cpp
${MAIN_ROOT_DIR}/main.cpp
${LOADER_ROOT_DIR}/loader.cpp
${SIMULATOR_ROOT_DIR}/simulator.cpp
${TOOLS_DIR}/logging.cpp
)

set(SIMULATOR_INCLUDE_DIRS
${MACHINE_ROOT_DIR}
${CACHE_DIR}
${BUILD_DIR_GEN}
${LOADER_ROOT_DIR}
${SIMULATOR_ROOT_DIR}
${TOOLS_DIR}
)

add_executable(Simulator ${SIMULATOR_SRC})
add_dependencies(Simulator generate_isa_handler)
target_include_directories(Simulator PRIVATE ${SIMULATOR_INCLUDE_DIRS})
target_compile_options(Simulator PRIVATE
$<$<CONFIG:Debug>:-g -O0>
$<$<CONFIG:Release>:-O2>
)

add_custom_target(clang-force-format
    COMMAND find "${INSTRUCTIONS_MAP_ROOT_DIR}" "${MACHINE_ROOT_DIR}" "${LOADER_ROOT_DIR}" "${SIMULATOR_ROOT_DIR}" "${CACHE_DIR}"
                -name "*.cpp" -exec clang-format -style='{ColumnLimit: 120, IndentWidth: 4, UseTab: Never}' -i {} +
)

add_custom_target(clang-force-format-h
    COMMAND find "${INSTRUCTIONS_MAP_ROOT_DIR}" "${MACHINE_ROOT_DIR}" "${LOADER_ROOT_DIR}" "${SIMULATOR_ROOT_DIR}" "${CACHE_DIR}"
                -name "*.h" -exec clang-format -style='{ColumnLimit: 120, IndentWidth: 4, UseTab: Never}' -i {} +
)

add_dependencies(clang-force-format clang-force-format-h)