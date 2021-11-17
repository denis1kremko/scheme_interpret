cmake_minimum_required(VERSION 3.17)
project(scheme_interpret)

set(CMAKE_CXX_STANDARD 17)

add_executable(scheme_interpret error.cpp functions.cpp parser.cpp scheme.cpp scope.cpp object.cpp)