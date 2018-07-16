cmake_minimum_required(VERSION 2.8)

project(antiEU)
add_executable(antieu AntiEu.cpp
point.cpp
Line.cpp
CompareLine.cpp)

set (CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
# set (CMAKE_CXX_STANDARD_REQUIRED TRUE) not work(
# set(CMAKE_CXX_STANDARD 11)

function(uselib exec module)
    find_package(${module} QUIET)
    if(NOT ${module}_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(${module} ${module} QUIET)
        if(NOT ${module}_FOUND)
            message(FATALERROR "Library not found: " ${module})
        endif()
    endif()
    
    message("Library found: " ${module})
    include_directories(${${module}_INCLUDE_DIRS} ${${module}_INCLUDE_DIR})
    target_link_libraries(${exec} ${${module}_LIBRARIES} ${${module}_LIBRARY}) 
endfunction()

# uselib(main GTK2)
# uselib(main gtkgl-2.0)
