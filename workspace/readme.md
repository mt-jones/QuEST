# Simulating quantum cellular automaton using QuEST
## Workspace ontology
```bash
qca/
  build-qca
  memcheck-qca
  qca.c
  data/
    readme.md
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
  qca/
    __init__.py
    qca/
      read/
        __init__.py
        density.py
        mutual.py
      run/
        __init__.py
        circuit.py
build-utilities/
  build-tool
```
* `build-qca` compiles the project.
* `memcheck-qca` runs the project through valgrind's memory profiler.
* `qca.c` defines the project's execution.
* `data` write directory for simulation data.
* `qca/external` specifies externally included dependencies.
* `qca/logging` defines parent logging functions used by the project.
* `qca/utilities` defines parent utility functions used by the project.
* `qca/rules` defines the evolution rules for a prescribed neighborhood size.
* `qca/qca` python module for orchestration.
* `build-utilities` defines build utilities for compiling QuEST projects.
## Compilation and execution
1. Clone this project.
2. Edit `build-qca` so that `QUEST_ROOT` is the absolute path to QuEST source, and the options are appropriate for the target system
3. Execute this command
```bash
cd qca/
./build-qca
```
Once complete, there will be a new directory called `build` that contains `exe_qca`. To run the program, execute this command:
```bash
./build/exe-qca rmajor=true verbose=false nrows=3 ncols=3 rule=12 activator=Sqrt-Hadamard ic=W ncycles=1
```

Alternatively, the python module can be used (from the `workspace/qca` directory):
```python
import qca
status = qca.run.circuit.cycle()
for stat in status:
  print(stat)
```

Typical output:
```bash
$ --------------------------------
$ | Printing command line inputs |
$ --------------------------------
$ ----------------
$ | rmajor: true |
$ ----------------
$ ------------------
$ | verbose: false |
$ ------------------
$ ------------
$ | nrows: 3 |
$ ------------
$ ------------
$ | ncols: 3 |
$ ------------
$ ------------
$ | rule: 12 |
$ ------------
$ ----------------------------
$ | activator: Sqrt-Hadamard |
$ ----------------------------
$ ---------
$ | ic: W |
$ ---------
$ --------------
$ | ncycles: 1 |
$ --------------
$ -------------------------------
$ | Printing simulation details |
$ -------------------------------
$ ------------
$ | Rule: 12 |
$ ------------
$ -----------------------------
$ | valid activation level: 3 |
$ -----------------------------
$ -----------------------------
$ | valid activation level: 2 |
$ -----------------------------
$ --------------------------
$ | Printing initial cond. |
$ --------------------------
$ -------------
$ | 0 | 1 | 0 |
$ -------------
$ | 1 | 0 | 1 |
$ -------------
$ | 0 | 1 | 0 |
$ -------------
$ --------------------------------------------------------
$ | Printing simulated quantum register and runtime info |
$ --------------------------------------------------------
$ QUBITS:
$ Number of qubits is 9.
$ Number of amps is 512.
$ Number of amps per rank is 512.
$ EXECUTION ENVIRONMENT:
$ Running locally on one node with GPU
$ Number of ranks is 1
$ OpenMP disabled
$ -----------------------
$ | Cycling the circuit |
$ -----------------------
$ ----------------------------
$ | Initial probability grid |
$ ----------------------------
$ -------------------------
$ |  0.00 |  1.00 |  0.00 |
$ -------------------------
$ |  1.00 |  0.00 |  1.00 |
$ -------------------------
$ |  0.00 |  1.00 |  0.00 |
$ -------------------------
$ --------------------------------
$ | Probability grid at cycle: 0 |
$ --------------------------------
$ -------------------------
$ |  0.25 |  1.00 |  0.25 |
$ -------------------------
$ |  1.00 |  0.00 |  1.00 |
$ -------------------------
$ |  0.25 |  1.00 |  0.25 |
$ -------------------------
$ -------------------------
$ | Finished; cleaning up |
$ -------------------------
```
