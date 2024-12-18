cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(Simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(INSTRUCTIONS_MAP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/instructions_map")
set(MACHINE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/machine")
set(SIMULATOR_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/simulator")
set(LOADER_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/loader")
set(TIMERS_INTERRUPTS "${CMAKE_CURRENT_SOURCE_DIR}/timers_interrupts")
set(MAIN_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/main")
set(BUILD_DIR_GEN "${CMAKE_BINARY_DIR}/generated")
set(CACHE_DIR "${MACHINE_ROOT_DIR}/cache")
set(TOOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tools")
set(PLUGINS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/plugins")
set(PLUGINS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/plugins")



if (NOT DEFINED PLUGIN_ENABLED)
     set(PLUGIN_ENABLED OFF)
 endif()

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
${TIMERS_INTERRUPTS}/timers.cpp
)

set(SIMULATOR_INCLUDE_DIRS
${MACHINE_ROOT_DIR}
${CACHE_DIR}
${BUILD_DIR_GEN}
${LOADER_ROOT_DIR}
${SIMULATOR_ROOT_DIR}
${TOOLS_DIR}
${TIMERS_INTERRUPTS}
)

add_executable(Simulator ${SIMULATOR_SRC})
add_dependencies(Simulator generate_isa_handler)
target_include_directories(Simulator PRIVATE ${SIMULATOR_INCLUDE_DIRS})
target_compile_options(Simulator PRIVATE
$<$<CONFIG:Debug>:-g -O0>
$<$<CONFIG:Release>:-O2>
)

if (PLUGIN_ENABLED)
   target_compile_definitions(Simulator PRIVATE PLUGIN_ENABLED)
endif()

add_custom_target(clang-force-format
    COMMAND find "${INSTRUCTIONS_MAP_ROOT_DIR}" "${MACHINE_ROOT_DIR}" "${LOADER_ROOT_DIR}" "${SIMULATOR_ROOT_DIR}" "${CACHE_DIR}"
                -name "*.cpp" -exec clang-format -style='{ColumnLimit: 120, IndentWidth: 4, UseTab: Never}' -i {} +
)

add_custom_target(clang-force-format-h
    COMMAND find "${INSTRUCTIONS_MAP_ROOT_DIR}" "${MACHINE_ROOT_DIR}" "${LOADER_ROOT_DIR}" "${SIMULATOR_ROOT_DIR}" "${CACHE_DIR}"
                -name "*.h" -exec clang-format -style='{ColumnLimit: 120, IndentWidth: 4, UseTab: Never}' -i {} +
)

add_dependencies(clang-force-format clang-force-format-h)

function(create_plugin_library plugin_source)
    get_filename_component(plugin_name ${plugin_source} NAME_WE)
    
    add_library(${plugin_name} SHARED ${plugin_source})

    target_include_directories(${plugin_name} PRIVATE ${SIMULATOR_INCLUDE_DIRS})
   
    file(MAKE_DIRECTORY "${PLUGINS_OUTPUT_DIR}")

    set_target_properties(${plugin_name} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "${plugin_name}"
        LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_OUTPUT_DIR}"
    )

    if(TARGET generate_isa_handler)
        add_dependencies(${plugin_name} generate_isa_handler)
    endif()

endfunction()

file(GLOB PLUGIN_SOURCES "${PLUGINS_DIR}/*.cpp")

foreach(plugin_source IN LISTS PLUGIN_SOURCES)
    create_plugin_library("${plugin_source}")
endforeach()