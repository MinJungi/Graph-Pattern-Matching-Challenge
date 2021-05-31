add_executable(program main.cc ${SOURCES})
set(CMAKE_EXE_LINKER_FLAGS -static)