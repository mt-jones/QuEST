# Simulating quantum cellular automaton using QuEST
## Workspace ontology
```bash
qca/
  build-qca
  memcheck-qca
  qca.c
  external/
    external.h
  logging/
    logging.h
  utilities/
    utilities.h
  rules/
    five-site/
      five-site.h
      logging.h
      utilities.h
    three-site/
build-utilities/
  build-tool
```
* `build-qca` compiles the project.
* `memcheck-qca` runs the project through valgrind's memory profiler.
* `qca.c` defines the project's execution.
* `qca/external` specifies externally included dependencies.
* `qca/logging` defines parent logging functions used by the project.
* `qca/utilities` defines parent utility functions used by the project.
* `qca/rules` defines the evolution rules for a prescribed neighborhood size.
* `build-utilities` defines build utilities for compiling QuEST projects.
## Compilation and execution
1. Clone this project.
2. Edit `build-qca` so that `QUEST_ROOT` is the absolute path to QuEST source.
3. Execute this command
```bash
cd qca/
./build-qca
```
Once complete, there will be a new directory called `build` that contains `exe_qca`. To run the program, execute this command:
```bash
./build/exe-qca
```
