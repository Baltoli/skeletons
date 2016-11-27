cmake_minimum_required(VERSION 2.8)
project(SimpleProject)

# Use the version of clang that has OpenMP support included
if(APPLE)
  set(CMAKE_C_COMPILER clang CACHE STRING "C compiler" FORCE)
  set(CMAKE_CXX_COMPILER clang++ CACHE STRING "C compiler" FORCE)
endif()
if(UNIX AND NOT APPLE)
  set(CMAKE_C_COMPILER /auto/groups/acs-software/L25/llvm/bin/clang CACHE STRING "C compiler" FORCE)
  set(CMAKE_CXX_COMPILER /auto/groups/acs-software/L25/llvm/bin/clang++ CACHE STRING "C++ compiler" FORCE)
endif()

find_package(LLVM REQUIRED)

if(NOT CMAKE_LLVM_CONFIG)
  set(CMAKE_LLVM_CONFIG llvm-config)
endif()

if(NOT CMAKE_LLVM_LIT)
  set(CMAKE_LLVM_LIT llvm-lit)
endif()

exec_program(${CMAKE_LLVM_CONFIG} ARGS --version  OUTPUT_VARIABLE LLVM_VERSION)

if("${LLVM_VERSION}" MATCHES "not found")
  message(FATAL_ERROR "llvm-config not found; may need to set PATH?")
endif()

exec_program(${CMAKE_LLVM_CONFIG} ARGS --includedir OUTPUT_VARIABLE LLVM_INCDIR)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --libdir   OUTPUT_VARIABLE LLVM_LIBDIR)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --src-root OUTPUT_VARIABLE LLVM_SRC)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --obj-root OUTPUT_VARIABLE LLVM_OBJ)

if(NOT CLANG_RESOURCE_DIR)
  exec_program(${CMAKE_LLVM_CONFIG} ARGS --bindir OUTPUT_VARIABLE LLVM_BIN)
  exec_program(${LLVM_BIN}/clang ARGS --version OUTPUT_VARIABLE VERSION)

  string(REPLACE "clang version " "" VERSION ${VERSION})
  string(REGEX REPLACE "\n.*" "" VERSION ${VERSION})
  string(REGEX REPLACE " .*" "" VERSION ${VERSION})

  set(CLANG_RESOURCE_DIR ${LLVM_OBJ}/lib/clang/${VERSION}
    CACHE PATH "Clang resource dir (stdarg.h, etc.)")
  message(STATUS "Found Clang resource dir: ${CLANG_RESOURCE_DIR}")
endif()

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${LLVM_PREFIX}/share/llvm/cmake")

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

exec_program(${CMAKE_LLVM_CONFIG} ARGS --cflags   OUTPUT_VARIABLE LLVM_C)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --cxxflags OUTPUT_VARIABLE LLVM_CXX)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LLVM_C}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wstrict-prototypes")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(skel
  src/main.cc 
)
foreach(i
	AST
	ASTMatchers
	Analysis
	Basic
	Driver
	Edit
	Frontend
	FrontendTool
	Lex
	Parse
	Sema
	Edit
	Rewrite
	RewriteFrontend
	StaticAnalyzerFrontend
	StaticAnalyzerCheckers
	StaticAnalyzerCore
	Serialization
	ToolingCore
	Tooling
	Format)
	target_link_libraries(skel clang${i})
endforeach(i)

add_executable(map
  examples/util.c
  examples/map.c
)
set_target_properties(map PROPERTIES COMPILE_FLAGS "-O3 -fopenmp")
if(UNIX AND NOT APPLE)
  target_link_libraries(map "/home/bsc28/opt/lib/libiomp5.so")
endif()
if(APPLE)
  target_link_libraries(map "/usr/local/lib/libiomp5.dylib")
endif()

message(STATUS ${CMAKE_LLVM_CONFIG})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support option mcparser bitreader
  profiledata)

# Link against LLVM libraries
target_link_libraries(skel ${llvm_libs})
