
cmake_minimum_required (VERSION 3.14)

project(SaveTWStockDataToDB)

add_executable(${PROJECT_NAME} "main.cpp" "${BACKEND_SOURCE_DIR}/stockdbhelper.cpp" "${BACKEND_SOURCE_DIR}/utility.cpp")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14 c_std_99)
target_link_libraries(${PROJECT_NAME} PRIVATE sqlite3 gflags curl json-c)
