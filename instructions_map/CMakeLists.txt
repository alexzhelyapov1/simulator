project(InstructionsMap)

set(CMAKE_GENERATOR Ninja)
cmake_minimum_required(VERSION 3.22)
add_custom_target(generate_c_code
    COMMAND ruby generate_instructions.rb
    DEPENDS riscv_instructions.yaml generate_insts.erb
    OUTPUT ./gen/gen_func.cpp

)