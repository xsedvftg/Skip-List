cmake_minimum_required(VERSION 3.5)

project(SkipList)

add_executable(${PROJECT_NAME}
    src/main.cc
    src/SkipList.cc
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)