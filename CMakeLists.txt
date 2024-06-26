cmake_minimum_required(VERSION 3.29)

# META
set(PROJECT_DESCRIPTION "Pacman package logger")
set(DESTINATION_PREFIX bin)

# CONFIG

# CHECK IF PACLOGGER_UPDATES_LOG IS EMPTY
if(NOT PACLOGGER_UPDATES_LOG)
  set(PACLOGGER_UPDATES_LOG "$ENV{HOME}/.cache/paclogger.log")
endif()

project(
  paclogger
  LANGUAGES CXX
  VERSION 1.2.2
  DESCRIPTION ${PROJECT_DESCRIPTION})
set(CMAKE_CXX_STANDARD 20)

# RECURSIVE FILE GLOBBING
file(GLOB_RECURSE sources src/*.cpp src/*.h)

# TARGET CREATION
add_executable(${PROJECT_NAME} ${sources})
target_compile_definitions(${PROJECT_NAME}
                           PRIVATE PL_ENV="${PACLOGGER_UPDATES_LOG}")

# MOVE EXECUTABLE TO OUTPUT FOLDER
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
          ${CMAKE_SOURCE_DIR}/out/${PROJECT_NAME}
  COMMENT "Symlinking executable to output folder")

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND strip -x -X -s -R .gnu.version -R .gnu.version_r.
          ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
  COMMENT "Stripping executable")

set(BINARY_OUTPUT_DIR
    "${CMAKE_INSTALL_PREFIX}/${DESTINATION_PREFIX}/${PROJECT_NAME}")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/paclogger.hook.in
               ${CMAKE_CURRENT_BINARY_DIR}/paclogger.hook @ONLY)

# INSTALLATION TO THE USER BIN FOLDER
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${DESTINATION_PREFIX})

# INSTALLING THE HOOK
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/paclogger.hook
        DESTINATION /usr/share/libalpm/hooks/)
