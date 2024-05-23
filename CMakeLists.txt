cmake_minimum_required(VERSION 3.29)

# META
set(PROJECT_DESCRIPTION "Pacman package logger")
set(DESTINATION_PREFIX bin)

project(paclogger LANGUAGES CXX VERSION 0.0.1 DESCRIPTION ${PROJECT_DESCRIPTION})
set(CMAKE_CXX_STANDARD 20)

# COMPILER FLAGS
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -std=c++${CMAKE_CXX_STANDARD}")

# COMPILER FLAGS DEPENDANT ON BUILD TYPE
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s -fno-ident -fno-asynchronous-unwind-tables -ffunction-sections -DNDEBUG")
	set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections")
endif()

# RECURSIVE FILE GLOBBING
file(GLOB_RECURSE sources src/*.cpp src/*.h)

# TARGET CREATION
add_executable(${PROJECT_NAME} ${sources})

# MOVE EXECUTABLE TO OUTPUT FOLDER
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/out/${PROJECT_NAME}
	COMMENT "Symlinking executable to output folder"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND strip -x -X -s -R .gnu.version -R .gnu.version_r. ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
	COMMENT "Stripping executable"
)

set(BINARY_OUTPUT_DIR "${CMAKE_INSTALL_PREFIX}/${DESTINATION_PREFIX}/${PROJECT_NAME}")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/paclogger.hook.in
	${CMAKE_CURRENT_BINARY_DIR}/paclogger.hook
	@ONLY
)

# INSTALLATION TO THE USER BIN FOLDER
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${DESTINATION_PREFIX})