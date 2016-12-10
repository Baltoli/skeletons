# skeletons

This project aims to detect and optimise the implicit usage of parallel
skeletons in a C program.

## Parallel Skeletons

A parallel skeleton can be thought of as a basic unit of parallel composition
from which larger programs and algorithms can be constructed. They aim to
provide a structured way in which sequential code can be incorporated into a
parallel framework (rather than, for example, mixing the two with no inherent
structure imposed, as might be the case for manually written OpenMP code).

There exist libraries that allow for programs to be annotated explicity in a way
similar to parallel skeletons, and have the code automatically parallelised
appropriately. In this project I aim to automatically detect parts of a program
that resemble a parallel skeleton.

An early paper on parallel skeletons ("Parallel Skeletons for Structured
Composition") describes a model in which programs in a sequential language are
composed together at a higher level of abstraction using a control language. In
it, the authors describe several "elementary" skeletons that provide the basis
for distributing work over a collection of data. These elementary skeletons will
provide the basis for the work done in this project.

## Building

Currently the project is in a state of "builds on my machine" unfortunately,
though it may be possible to get it to build elsewhere.

Prerequisites:

  * Clang and LLVM built and installed from source somewhere - I think the LLVM
    discovery module for CMake should pick it up from there.

    mkdir Build
    cd Build
    cmake ..
    make

## Usage

The project is implemented as a Clang tool, and so can make use of _compilation
databases_ if necessary. To run the tool on a single source file that doesn't
need any complex build flags to be compiled:

    skel filename --

Eventually `skel` will be set up to automatically annotate source files with
OpenMP pragmas that allow for parallel execution. For now, successes are
reported on `stdout` with line numbers marking where a match has been found, and
errors and debug messages go to `stderr`.

## Implemented Skeletons

These are the skeletal shapes that can currently be detected by `skel`.

### Map

A map skeleton takes a function `f` and applies it to every element of an input
array, copying the results into another array. The current criteria for
detecting this skeleton in source code are demonstrated by this code sample:

    for(int i = 0; i < N; i++) {
      // ... code before
      target[i] = func(source[i]);
      // ... code after
    }

The before and after code is checked to ensure that there are no other
assignments to the target (at *any* index) - if there are, the loop will not be
annotated, and a near miss reported. This is obviously overly strong, but as a
first analysis it's OK.
