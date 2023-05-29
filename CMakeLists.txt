cmake_minimum_required(VERSION 3.23)
project(CityNetwork)

set(CMAKE_CXX_STANDARD 17)

add_executable(CityNetwork src/main.cpp src/App.cpp src/App.h src/CSVReader.cpp src/CSVReader.h src/CityNetwork.cpp src/CityNetwork.h)
