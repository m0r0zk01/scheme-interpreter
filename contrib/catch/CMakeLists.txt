add_library(catch STATIC catch_main.cpp)
target_include_directories(catch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
