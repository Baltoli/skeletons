# Progress Notes

# Initial Experiments & Motivation

As is intuitive, it's very hard to actually achieve a speedup by using a naive
OpenMP pragma on a for loop. In some cases it does seem to be possible, but a
good sequential compiler will beat it in most cases.

Using the 32-core ACS machines and gcc 4.8, the parallel implementation is a bit
faster than the sequential. Using any new version of clang (especially on less
core-y machines like my laptop), the sequential version is much, much faster.
Worth investigating if clang is doing some kind of optimisation funny business
to get rid of the sequential loop?

Shenanigans seem possible! clang-omp on O0 runs the parallel version faster, but
on O3/Ofast, the sequential implementation takes 0.00000s.

Proof (of a sort). On my laptop, *accessing the arrays* after performing the map
causes the sequential version to actually do some work! Now even on my laptop,
which doesn't have ideal concurrency characteristics, when performing more work
than a trivial multiplication (on a big enough data set!), the parallel version
is faster.

# Discovering Maps

People don't write code like this in real life. Most useful outcome is that I
know how to write a clang tool!

In light of this, my next step will be to read through some of the source of the
GNU Scientific Library to identify more "real-world" patterns in heavily
numerical code.

GSL picked for a number of reasons:

  * CMake available for building (so I can very easily make a compilation
    database).
  * Large enough code base that patterns I'm looking for are likely to occur
    somewhere (hopefully).

New approach - find some patterns in the code base that can be generalised into
something I can detect in a way similar to the map matcher.

## GSL Findings

  * Code very rarely assigns directly to an array or pointer index. A lot of the
    time, the assignment is done through a layer of indirection.
