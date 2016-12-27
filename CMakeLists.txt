cmake_minimum_required(VERSION 2.6)

project(Skeletons)

# Include LLVM configuration:
find_package(LLVM REQUIRED)
include(AddLLVM)

if(NOT CMAKE_LLVM_CONFIG)
	set(CMAKE_LLVM_CONFIG llvm-config)
endif()

if(NOT CMAKE_LLVM_LIT)
	set(CMAKE_LLVM_LIT llvm-lit)
endif()

# Make sure we can find llvm-config!
exec_program(${CMAKE_LLVM_CONFIG} ARGS --version  OUTPUT_VARIABLE LLVM_VERSION)
if("${LLVM_VERSION}" MATCHES "not found")
  message(FATAL_ERROR "llvm-config not found; may need to set PATH?")
endif()

exec_program(${CMAKE_LLVM_CONFIG} ARGS --includedir OUTPUT_VARIABLE LLVM_INCDIR)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --libdir   OUTPUT_VARIABLE LLVM_LIBDIR)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --src-root OUTPUT_VARIABLE LLVM_SRC)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --obj-root OUTPUT_VARIABLE LLVM_OBJ)
exec_program(${CMAKE_LLVM_CONFIG} ARGS --cxxflags OUTPUT_VARIABLE LLVM_CXX)

# Find the Clang "resource dir" (contains include/stdarg.h, etc.).
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

# Explicitly put LLVM's library directory before others, e.g. /usr/local/lib.
link_directories(${LLVM_LIBDIR})

# Always build with all warnings.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wstrict-prototypes")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#
# TESLA uses C++11.
#
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXX}")

#
# Global include directories for all of TESLA:
#
include_directories(${LLVM_INCDIR})
# Add includes from src and obj only if we're building from a
# non-installed copy.
if (${LLVM_SRC}/include STREQUAL ${LLVM_INCDIR})
	include_directories(
		${LLVM_OBJ}/include
		${LLVM_SRC}/tools/clang/include
		${LLVM_OBJ}/tools/clang/include
	)
endif()
include_directories(
	include                          # [lib]tesla.h
	${CMAKE_BINARY_DIR}/include      # config.h
)

add_llvm_executable(skel
  src/main.cc
  src/DetectSkeletonsAction.cc
  src/DetectDynamicAction.cc
  src/AnnotateSkeletonsAction.cc
  src/DetectSkeletonsASTConsumer.cc
  src/DetectDynamicASTConsumer.cc
  src/MapMatcher.cc
  src/Log.cc
)
foreach(i
	Analysis AST ASTMatchers Basic Driver Edit Frontend Lex Parse
        Rewrite Sema Serialization Tooling
)
	target_link_libraries(skel clang${i})
endforeach(i)

llvm_map_components_to_libraries(LLVM_LIBS bitreader mcparser)
target_link_libraries(skel ${LLVM_LIBS} tinfo)

option(ENABLE_TESTS "Enable the tests")

if(ENABLE_TESTS)
  enable_testing()
  add_subdirectory("shapes")
endif()
