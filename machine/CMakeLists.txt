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
)

add_executable(Machine ${MACHINE_SRC})
target_include_directories(Machine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${CACHE_DIR})