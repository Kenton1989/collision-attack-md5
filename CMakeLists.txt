cmake_minimum_required(VERSION 3.12)

project(MD5_attack)
include_directories(${PROJECT_SOURCE_DIR}/src)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/md5-reference-impl SEC_LIST)

add_executable(${PROJECT_NAME} ${SEC_LIST})