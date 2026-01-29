cmake_minimum_required(VERSION 3.28)

# -------------------------------
# Project name and main cpp
# -------------------------------
if(NOT DEFINED PROJECT_NAME)
    set(PROJECT_NAME "MyProject")
endif()

project(${PROJECT_NAME} LANGUAGES CXX)

if(NOT DEFINED MAIN_CPP)
    set(MAIN_CPP src/main.cpp)
endif()

# -------------------------------
# C++ standard
# -------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# Output directory
# -------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# -------------------------------
# Collect source files
# -------------------------------
file(GLOB_RECURSE CPP_FILES ${CMAKE_SOURCE_DIR}/lib/*.cpp)

    
    
# message(STATUS "Downloading nlohmann/json (this may take a few seconds)...")
# # This prints progress from git automatically
# FetchContent_MakeAvailable(nlohmann_json)
# message(STATUS "nlohmann/json downloaded successfully ✅")


# -------------------------------
# Create executable
# -------------------------------
add_executable(${PROJECT_NAME}
    ${MAIN_CPP}
    ${CPP_FILES}
)

# Include your headers
target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

# Link nlohmann/json
# target_link_libraries(${PROJECT_NAME}
#     PRIVATE nlohmann_json::nlohmann_json
# )
# target_include_directories(${PROJECT_NAME}
#     PRIVATE ${CMAKE_SOURCE_DIR}/include
# )


# -------------------------------
# Compile options
# -------------------------------
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:RelWithDebInfo>:-O2 -g>
    $<$<CONFIG:MinSizeRel>:-Os>
)
