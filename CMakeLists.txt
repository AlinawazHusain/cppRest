cmake_minimum_required(VERSION 3.28)

project(CppRest)

add_executable(${PROJECT_NAME} src/main.cpp src/http_server.cpp src/json.cpp src/jwt.cpp)