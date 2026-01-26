cmake_minimum_required(VERSION 3.28)

if(NOT DEFINED PROJECT_NAME)
    set(PROJECT_NAME "MyProject")
endif()

project(${PROJECT_NAME})

# Main cpp file passed from bash
if(NOT DEFINED MAIN_CPP)
    set(MAIN_CPP src/main.cpp)
endif()



# Set the folder where executables will go
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB CPP_FILES ${CMAKE_SOURCE_DIR}/lib/*.cpp)


add_executable(${PROJECT_NAME}
    ${MAIN_CPP}
    ${CPP_FILES}
)


target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
    $<$<CONFIG:MinSizeRel>:-Os>
)