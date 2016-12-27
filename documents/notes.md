# Progress Notes

## Initial Experiments & Motivation

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

## Discovering Maps

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

### GSL Findings

  * Code very rarely assigns directly to an array or pointer index. A lot of the
    time, the assignment is done through a layer of indirection.
  * GNU code seems to have an aversion to inline for loop variables (as the code
    was probably written before 1999!).

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

### Commutativity

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
  * However, a more sophisticated version might consider examining syscalls that
    each version of the program makes. It would be worth a reasonably sized
    examination of the pros and cons of this kind of system - for example, how
    would you want this kind of system to interact with code that might be
    considered to be "dangerous" in some sense? You wouldn't want your compiler
    to make demons fly out of your nose as part of the compilation process.
  * Output comparison is probably the best way to look at this for simple
    programs. In order to look at how this interacts with "real world" code
    like GSL, I'll have to write some test harnesses that call into the
    appropriate code paths and output text. Should also come up with some
    "adversarial" examples that are not parallelisable so that the system's
    safety can be demonstrated.

So we want a system that can reorder loop iterations to discover parallelisable
loops dynamically. There will be a static analysis component to this to discover
the loops initially (probably using the Clang tool setup that I already have in
place for the project), but this will be much more permissive in what it will
accept compared to the initial attempt at a map matcher.

In order to come up with a reasonable implementation of an iteration reorderer,
I think that having multiple candidate strategies is a sensible idea (to get
good coverage on possible problems). For example, we might try reversing the
iterations, doing odd then even, or (for a fixed upper bound) randomly ordering
them. There is also a "second order" technique that could be used here - the
random analysis is the most likely to be useful, but it only applies in the case
where the upper bound is constant. If we instrument the loop bound variable and
find it to be constant "in practice", then a further attempt could be made using
that value to randomise the ordering.

Loops that will be eligible for this treatment will need to be loops that have
fixed upper and lower bounds. The strategies will then involve a partial
unrolling of the loop, changing the effective order of iterations. For example:

    for(int i = 0; i < N; i++) {
      work(i);
    }

might be reordered to something like:

    for(int i = 0; i < N; i += 2) {
      work(i);
    }
    for(int i = 1; i < N; i += 2) {
      work(i);
    }

so that all the even numbered iterations will execute before all of the
odd-numbered ones.

For each strategy, the tool will generate a different version of the affected
source file. The overall program can then be recompiled with each of these
versions individually, and a script can be used to run each version and compare
the outputs.

If the script finds that the programs behave identically under the conditions
given, then the tool can be used in another mode (with an extra input indicating
the loop to be annotated?) to actually perform the optimisation.

The system being built here will be composed of more than a Clang tool to do the
discovery of loops. It will also require:
* Some way of programatically recompiling a program with a single source file
  replaced by reordered variants. This should be reasonably easy to achieve if
  using CMake to do the compilation. However, it might be trickier if the loop
  being parallelised is in library code (e.g. GSL).
* Method for comparing the output of programs to see if they are equivalent
  (this can probably just be a shell script that runs them all and does naive
  comparison on the outputs).
* Clang tool will need to do both loop discovery and annotation.
* The loop discovery phase will need to output a file that gives the location
  within the file that a potential candidate loop has been found.
* The source file will need to have some kind of annotations added to signify
  that a particular loop has already been looked at and does not need to be
  analysed again.

Something to think about - this process means that source files are being
generated as part of the build process. Will probably need some CMake
shenanigans to note which are to be analysed in this way as part of the build
process.

Alternative is to start off with this being a mostly manual process, and if
there's time, convert it to a more automated version using CMake. This might be
the better idea for now.

### Implementation Strategy

First part is quite similar to the existing static analysis component. We want
to identify for loops that have a "simple" structure. This will probably be best
done by a Clang AST matcher that is less strict than the map matcher. The result
of this step could be simply to annotate the source files with a comment above
loops that should be tested using the next steps in the procedure.

What do potentially reorderable loops look like? The bulk of the decision is
done on the init, condition and increments. These should be:
* Init: needs to initialise a variable to a constant value (that might not be
  zero). Don't support variable initialisation values for now.
* Condition: should check that the same variable as declared in the initialiser
  is being compared to some fixed value (variable or constant).
* Increment: ++ on the same variable as the other two.

As well as these, we also need to check that the body of the loop doesn't write
to the loop variable or to the bound condition (if it is indeed a variable).

If the loop meets these conditions, then we should add a fixed comment above it
in the source file (e.g. "LOOP-ORDER: CHECK"). This will act as a marker for
future runs of the tool (so that they can know which loops to analyse).
