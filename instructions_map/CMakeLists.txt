project(InstructionsMap)

set(CMAKE_GENERATOR Ninja)
cmake_minimum_required(VERSION 3.22)
make_directory(${BUILD_DIR_GEN})
add_custom_target(generate_isa_handler
    COMMAND ruby ${INSTRUCTIONS_MAP_ROOT_DIR}/process_instrs.rb --sourse_dir "${INSTRUCTIONS_MAP_ROOT_DIR}/" --gen_dir "${BUILD_DIR_GEN}/" 
)