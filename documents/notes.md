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

In the future it might be nice to look at whether the work on for loops that is
shared between the static analyser and the reorderable loop finder could be
shared rather than duplicated. For now I think it's fine as they're doing
conceptually different things and might have differing implementations in the
future.

Currently the reorderable finder discovers loops that have the for loop
conditions discussed previously. The static analysis for "no writes to loop
variable or bounds" might not actually be necessary - changing these in a loop
iteration is likely to just break things anyway, and if it doesn't then the
dynamic analysis has discovered a case that static analysis would have discarded
by being too conservative!

This should be a key point in the writeup I think - it might be tempting to
implement static analyses because they seem conceptually simple or innocent, but
if time is not a concern, then you might as well dynamically analyse them (for
the reasons discussed above).

Might actually need a more general approach. Let's try relaxing static loop
discovery constraints one by one until we start to find things in GSL that we
can apply the analysis to reasonably.

Coding style problem that might be good to write up - I have been implicitly
going by C99 style where variables are initialised inline with the for loop.
This does not always hold in legacy code, and we should be sensitive to this.

Now need to work out what the best way will actually be when "passing back" the
loops that need to be annotated. Options are:
* Just do the reordering there and then
  * Advantages:
    * Simpler code, as we already have references to the `ForStmt` etc. Can just
      do in-place copying of code blocks with mutated arguments.
  * Disadvantages:
    * Tool becomes more strongly coupled. Thinking about it, this might actually
      be OK.

Now that I look at it, doing the modifications in place will probably be the
better option (rather than marking with something for future reference).

Clang tools don't seem to have a convenient interface for copying a file rather
than doing destructive edits. Let's run with that as the interface, and use a
script to drive the copying of files (to keep the Clang tool code as simple as
possible). Will need to have a way of using CL flags to specify the loop
reordering strategy to be run on loops found.

The script will then be able to copy the source file several times, then run the
rewriter on each one individually.

So Clang provides Tree Transformations to allow an AST to be transformed. This
looks like what we want to be doing (rather than hacking together an ad-hoc
method).

Seems to be a very great deal of Clang gymnastics needed to actually use the
tree transform utilities. Why not try to do this as a source to source
transformation? Have a method somewhere that takes a ForStmt and returns a
string representing the reordered loop. Can then use the replace methods to slot
this in instead of the actual loop.

In the very long term this is almost certainly a worse approach, but it ought to
work in this case.

Note that the formatting erros caused by me assembling code like this can (and
should) be fixed by using clang-format.

Why does the tool segfault if the intermediate result is assigned to a variable
before passing it into Tool.run()? Only thing I can think of is something screwy
with ownership semantics etc.

### Strategies

**Loop reversal:** We want a loop to iterate backwards instead of forwards. For
example:

    for(int i = 0; i < N; i++) {
      work(i);
    }

will become:

    for(int i = N - 1; i >= 0; i--) {
      work(i);
    }

Some things that I will need to work out from the for loop are:
* Whether this loop can actually be reversed or not:
  * Needs an upper bound on the loop variable. The condition needs to be a
    binary operator, and the upper / lower bound will be the LHS of this
    operator. The AST matcher should take care of this for us, so we should just
    have to extract the relevant components.
  * The AST matcher restricts us to loops that have a declaration or a direct
    assignment to a variable as their initialisation statement. Use `isa` tests
    to get the loop variables out of these as appropriate.
  * The loop increment matcher currently only matches `++`, but could easily be
    extended to work on `--` in the future (along with a way of reversing loop
    condition checks).

Once we have the loop variable, bound and condition, the next step is to
reconstruct the reversed loop out of the components.

Now have a PODO that contains loop metadata along with a pointer to the clang
ForStmt containing all the other loop data. In DynamicHandler I can use the
ASTMatcher to bind a name to the AST node containing the loop condition, then
pass that as well.

Should change the interface to this PODO a bit. The reorderer already knows how
to turn a statement into a string, so we should in fact just pass the statements
bound by the matcher into the constructor rather than the strings we find. Also
need to pass the initialiser. The loop variable should just be passed as a
string though, as it's the only thing that doesn't fit neatly into a Stmt (as
the bound, initialiser etc. do).

Should add the ability to reverse loops other than increasing loops to the loop
reverser - currently can only reverse < / <= and ++. Would be useful to do > /
>= / --.

## Next Steps

So we currently have a system for reordering loops (in a semi-generic way),
along with a script that allows for a file to be copied and reordered using
strategies. The next steps will be to:
* Implement some more reordering strategies (to get better coverage of the
  possible subtleties that might be hidden by the code)
* Build some example programs (using GSL?) so that evaluation can be started.
* This system currently operates at a file level. Obviously not every loop in a
  file can be reordered - some might be possible while others might not be. This
  is obviously an iterative process, and so we need some mechanism for making it
  workable for a programmer.
* The minimum possible solution for making this an acceptable tool for a
  programmer to use is some way to report iterative feedback to the programmer.
  For example, this might involve them marking a particular loop as
  parallelisable or not (how to best do this marking in the source file?)
* One possible way to do the marking is to define a couple of macros (PARALLEL /
  NO_PARALLEL) that expand to annotation attributes (but a loop can't be
  directly annotated).
* My best here so far is to define a macro `NOPAR` that adds a redundant `,0`
  onto the end of the for loop's specifier, breaking the AST matcher when run on
  it (turns the final part into a binary rather than a unary operator).
* The programmer will have to perform a manual debugging process through marking
  functions as par / nopar.
* This isn't really ideal, as the compiler should really be doing more work
  here. How about:
  * Tool exits as soon as it finds *one* reorderable loop (reporting file, line
    numbers etc.). Should also output an identifier of some sort? 
  * The build script can then be used to automatically compile the project with
    each strategy applied.
  * Each compiled version can then be automatically run using the run script,
    and the outputs compared. If all are the same, then use a script to insert a
    macro at the source location reported previously (MAYBE_PAR). If they are
    not, insert NO_PAR:
    
        for(int i = 0; i < N; i++) {
          work(i);
        }

        MAYBE_PAR_FOR(int i = 0; i < N; i++) {
          work(i);
        }

        NO_PAR_FOR(int i = 0; i < N; i++) {
          work(i);
        }

        PAR_FOR(int i = 0; i < N; i++) {
          work(i);
        }

    where:
        
        #define MAYBE_PAR_FOR(x) for (x,0)
        #define NO_PAR_FOR(x) for (x,0)
        #define PAR_FOR(x) _Pragma("omp parallel for") for (x,0)
  * This strategy means that anything annotated with these macros will be
    ignored in the future by the AST matcher (downside is code *may* break if
    it's weird enough - this is OK). Then when it's run the next time, another
    loop will be discovered iteratively (run until no more are discovered!).
  * Next thing to work on is probably build script integration - need a way to
    programmatically compile variants of a file into multiple executables. CMake
    will be awful for this, so probably just a little shell script is the best
    way. Write up in evaluation about how the non-traditional build model means
    that integration with existing tools is borderline terrible.
  * This approach of using a manual script is stupid. Let's try to use a
    Makefile instead (this is what they're for, after all!)

Current status: I now have a build script that will automatically generate
reordered code and compile another version of the application. Next step will be
to make the tool exit after the *first* reordered loop, so that we can have a
more interactive process.

The type of every reordered loop is int for now. This might break weird code but
that can be fixed in the future.

The tool can now report the line at which it has reordered a loop. Next step is
to modify the reordering script so that it reports the line number and the file
in which the loop was found. This should go out to a file so that o

The annotation script can now read in a file as output by the reordering script.
The final step in this convoluted build process is the runner script. It will
need to take the manifest file, the base executable name and the list of
strategies. It can then run each executable and compare the results. As soon as
one is unequal, call annotate.sh to put in a `NO_PAR_FOR`. Otherwise, call
annotate to put in a `MAYBE_PAR_FOR`.

Usage note - currently no validation is done to ensure that every manifest file
produced is the same (running the makefile will produce a new one for each
incantation, but in theory they match so it's OK).

Back from a few days off. Next step should probably be to write some example
programs that can be compiled using this terrible workflow (demonstrating that
the parallelism in contrived examples can in fact be discovered).

Need to think about what kind of example will be amenable to doing this kind of
parallelism. Could implement some kind of numerical algorithm that will operate
on large arrays of data doing transformations. Might be best to try it on a
simple example for now to validate the workflow. Simplest possible example:
summing over a large array.

Ergonomics of the workflow probably aren't great at the moment - it requires
copying of scripts etc. Where do scripts need to be edited currently after
copying?
  * List of sources and experiment to build in the Makefile
  * Macros need to be changed depending on the parallelisation strategy (e.g. to
    add an OpenMP pragma)

Note for writeup in the future: nested loops pose a problem for the workflow as
it is at the moment. For example, consider:

    for(int i = 0; i < N; i++) {
      for(int j = 0; j < M; j++) {
        work(i,j);
      }
    }

This is OK as long as loops are evaluated *and parallelised* fully before
another loop is considered. If both the inner and the outer loop are marked as
'maybe', but only one of the two could possibly be parallelised, then it is
plausible that the user could end up in a situation where the tool has reported
that parallelism is safe when it is not. Write this up - important "gotcha" with
the tool as it currently stands.

Sequential loop reorderings (fundamentally) cannot detect problems caused by the
interleaving of loop iterations. Consider, for example, the problem of
iterations updating a shared variable (as the sum example does). There is
obviously no problem with sequential reorderings doing this out of order, as
they can't interfere with it. However, parallel reorderings can potentially
interfere with each other. How should this issue be addressed?
  * Hand the issue off to the programmer as part of their examination of the
    parallelism in the program. If there is shared state accessed, then they are
    responsible for noticing and explicitly annotating the loop as being
    parallel. All the tool has done is observe that some sequential reorderings
    of the loop preserve the externally observable behaviour of the program.
  * Use a more complex reordering strategy that affects writes inside the loop
    somehow? This goes a little bit beyond what the tool is supposed to do.
  * Give more useful diagnostics and documentation to the user - e.g. mentioning
    the need to protect shared state by an atomic variable.
Note that there are actually algorithms that are better behaved with regard to
parallel performance than the simple sum algorithm implemented so far. As long
as the read / write sets accessed by each iteration are independent (as well as
the behaviour remains the same), then they can be safely parallelised. It will
probably be worth mentioning the read / write set analysis in the writeup as a
potential extension that adds a great deal of usefulness to what is currently a
rather simple analysis. To do so would require some degree of fiddling with the
memory allocator etc. - how would you guarantee anything about stack addresses
for two separately compiled applications?

The simple sum experiment should actually provide a pretty good set of examples
that describe the tool well:
  * A cumulative sum in which the sum algorithm itself is not parallel, but many
    calls are made to copy the cumulative sum (modulo some bound) into another
    array. It can be pointed out that the programmer is aware that each
    iteration is independent of the others in terms of memory written to (each
    will only write to a single location in the output array, and the input
    array is read-only). Also gives a good jumping-off point for talking about
    more clever analyses (e.g. syscalls and memory read / written).
  * The normal sum is a good example of something marked as 'maybe' but is
    actually not strictly parallelisable in its current form (without marking as
    atomic).
  * The random fill should be one that's not parallelisable at all (although
    needs a separate example, because the sum of the arrays needs to be
    deterministic). Maybe better to remove the random fill from the sum
    experiment, and implement a deterministic fill? 

Should be obvious but it's worth noting that programs run using this dynamic
instrumenter need to be pure / deterministic in terms of what they output
(fill.c serves as a good example of this).

Last remaining part of the writeup to do is completing the evaluation to
give results based on GSL. Need to actually apply the techniques to one
of the GSL modules and get some results to write up in the remaining 500
words. Then go back to the writeup and redraft any bits that sound
awkward.
