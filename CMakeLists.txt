cmake_minimum_required (VERSION 3.14)

project ("stock")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(BACKEND_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories(inc /usr/include /usr/local/include submodules)

add_subdirectory ("src/SaveTWStockDataToDB")
add_subdirectory ("src/GetTWStockDataFromDB")
