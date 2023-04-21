Copywrite 2023, Gerald Ward

Code defines custom data structures for a vertex (node), edge (connectors between nodes), and adjacency list graph.

Taking an input file, the executable calculates the minimum cut of the graph utilizing a tailored verion of Kargers minimum cut graph algorithm. For example, take the following graph:

1 - 3   5 - 6
 \ /     \ /
  2 - - - 4

Minimum cut of above is '1', between vertices 2 and 4, since a clean split of the graph can be made at this junction that only causes a break in a single edge/connector.

Inputs for the graph are taken via .txt files, and (for now) passed hard-coded in the 'main' function.

Future revisions of this codebase will do the following (as of Apr 2023)
1. Split code into multiple scripts (vertex, edge, graph, main/kargerCalc)
2. Implement CMake
3. Implement smart pointers
4. Verify appropriate data structures utilized.
5. Ensure no memory leaks through 'valgrind -v' on executable
6. Update readme documentation for how to orient input text file
7. Pass text file input at runtime instead of hardcode in main (?)
8. Update print statements, creating debug flags for printing add'l params, and cleaning up output of executable
