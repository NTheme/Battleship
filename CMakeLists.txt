cmake_minimum_required(VERSION 3.23)
project(battlesh1p VERSION 1.2.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(SRC_PATH src/)
set(SOURCES ${SRC_PATH}/button.cpp 
            ${SRC_PATH}/cell.cpp 
            ${SRC_PATH}/command.cpp 
            ${SRC_PATH}/common.cpp 
            ${SRC_PATH}/field.cpp 
            ${SRC_PATH}/game.cpp 
            ${SRC_PATH}/network.cpp 
            ${SRC_PATH}/object.cpp 
            ${SRC_PATH}/player.cpp 
            ${SRC_PATH}/push.cpp 
            ${SRC_PATH}/ship.cpp 
            ${SRC_PATH}/window.cpp 
            main.cpp)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++20 -Og -Wall -Werror -fsanitize=address -Wl,-rpath-link,/usr/bin/x86_64-linux-gnu/libFLAC.so.8")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++20 -O3 -Wall")

find_package(SFML 2.5.1 COMPONENTS window graphics audio network system REQUIRED)
add_executable(battlesh1p ${SOURCES})
target_link_libraries(battlesh1p PRIVATE sfml-window sfml-graphics sfml-audio sfml-network sfml-system sfeMovie)

install(TARGETS battlesh1p DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY share DESTINATION ${CMAKE_INSTALL_PREFIX})
