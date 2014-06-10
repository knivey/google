project(google)

cmake_minimum_required(VERSION 2.8)

add_subdirectory(jansson)

include_directories(build/jansson/include)

add_executable(google main.c just_string.c)
TARGET_LINK_LIBRARIES(google curl jansson)

install(TARGETS google RUNTIME DESTINATION bin)
