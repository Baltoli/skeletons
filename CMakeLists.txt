cmake_minimum_required(VERSION 2.8.12)
project(SimpleProject)

# Use the version of clang that has OpenMP support included
set(CMAKE_C_COMPILER /auto/groups/acs-software/L25/llvm/bin/clang CACHE STRING "C compiler" FORCE)
set(CMAKE_CXX_COMPILER /auto/groups/acs-software/L25/llvm/bin/clang++ CACHE STRING "C++ compiler" FORCE)

# Find and set up LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM headers
include_directories(${LLVM_INCLUDE_DIRS})
include_directories(~/opt/include)
add_definitions(${LLVM_DEFINITIONS})

# Build the main executable for the project, linked against clang tooling
# libraries for now
add_executable(skel
  src/main.cc 
)
foreach(i Analysis AST ASTMatchers Basic Driver Edit Frontend Lex Parse Sema Serialization Tooling)
  target_link_libraries(skel clang${i})
endforeach(i)

# Build the example programs, using the flag to enable OpenMP and link manually
# (this is a bit of a hack) against the runtime support
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

# The main tool targets C++11
set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_C_FLAGS "-std=c11")

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(skel ${llvm_libs})
