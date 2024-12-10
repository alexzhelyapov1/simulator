project(Machine)

set(CACHE_DIR cache)

set(MACHINE_LIB
${CMAKE_CURRENT_SOURCE_DIR}/hart.h
${CMAKE_CURRENT_SOURCE_DIR}/machine.h
${CMAKE_CURRENT_SOURCE_DIR}/mem.h
)

set(MACHINE_SRC
${CMAKE_CURRENT_SOURCE_DIR}/machine.cpp
${CMAKE_CURRENT_SOURCE_DIR}/hart.cpp
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
${LOADER_ROOT_DIR}/loader.cpp
${TOOLS_DIR}/logging.cpp
)

add_executable(Machine ${MACHINE_SRC})
add_dependencies(Machine generate_isa_handler)
target_include_directories(Machine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${CACHE_DIR}
    ${CMAKE_BINARY_DIR}/${GEN_DIR}
    ${BUILD_DIR_GEN}
    ${LOADER_ROOT_DIR}
    ${TOOLS_DIR}
)