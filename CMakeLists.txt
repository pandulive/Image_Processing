
cmake_minimum_required(VERSION 3.1...3.31)

project(pandu VERSION 1.0)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")
set( OPENSCOPE-SRC
        src/main.cpp
        )

add_executable(${PROJECT_NAME} WIN32 ${OPENSCOPE-SRC})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_link_libraries(${PROJECT_NAME}  ${OpenCV_LIBS} )


if( MSVC )
    if(${CMAKE_VERSION} VERSION_LESS "3.6.0")
        message( "\n\t[ WARNING ]\n\n\tCMake version lower than 3.6.\n\n\t - Please update CMake and rerun; OR\n\t - Manually set 'GLFW-CMake-starter' as StartUp Project in Visual Studio.\n" )
    else()
        set_property( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME} )
    endif()
endif()
