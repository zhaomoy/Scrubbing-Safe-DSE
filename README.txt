Scrubbing-Safe Dead Store Elimination
================================

This implementation is based on LLVM 3.9.0. For that version of LLVM, only the dead store elimination pass (in lib/Transforms/Scalar/DeadStoreElimination.cpp) can remove scrubbing operations, so most of changes are made to this file.

Here are four sanitizer options I added: 
1) sec-dse 
The scrubbing-safe DSE

2) no-dse 
Turn off DSE completely.

3) byte-counter
When this option is enabled, whenever sec-dse keeps a dead store, instructions will be inserted to print out how many bytes that dead store is.
This option should be used with sec-dse

4) byte-counter-all
When this option is enabled, whenever dse removes a dead store, instructions will be inserted to print out how many bytes that dead store is.

The latter three options are for benchmarking and comparing dse and sec-dse.

To use any of the options above, set -fsanitize=[option name] when you compile your program.
