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

# New Ideas

## Dynamic Analysis

Static analysis might be too difficult to get good results out of in the time
that I have available to me. However, it might be possible to implement an
interesting dynamic analysis to discover potential parallel shapes in an
execution (for example, a map or a task farm).

The static analysis that I've implemented so far would make a good section in
the writeup demonstrating the problems with static analysis, along with some
examples from GSL that show "near misses" for my Clang tool's search for
parallel instances.

Then aim to implement dynamic analyses that do a similar job but better in some
way, and compare them to the limited static approach.

## Commutativity

Idea is to check whether iterations of a loop can be reordered safely - if they
can, then it's likely that the loop can be parallelised safely. This is a
dynamic analysis so there is no formal proof that the method works - it will
only be able to report a possible success back to the programmer, who can then
annotate their code manually (or take advantage of an option provided by the
tool to automatically do so).

Things to think about with this analysis:
* Data flow through the compiler and execution flow - how does the runtime
  information get back from a program execution to the compiler?
* Potentially (very much) longer run times for the compilation process - doesn't
  fit well into an existing iterative workflow.
* How to quanitfy that a program doesn't behave differently?
  * In this case, a simple comparison of `stdout` might be the best way to do
    it. 
