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
  * `clang-omp` installed - on mac OS this can be done using homebrew.

## Usage
