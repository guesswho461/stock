
cmake_minimum_required (VERSION 3.14)

project(GetTWStockDataFromDB)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "${BACKEND_SOURCE_DIR}/stockdbhelper.cpp" "${BACKEND_SOURCE_DIR}/utility.cpp" "${CMAKE_SOURCE_DIR}/submodules/mongoose/mongoose.c" "${BACKEND_SOURCE_DIR}/routefunctions.cpp")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14 c_std_99)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3 gflags nlohmann_json::nlohmann_json)