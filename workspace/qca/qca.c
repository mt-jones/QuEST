#include "external/external.h"
#include "rules/five-site/five-site.h"

int main() {
    char buffer[80];
    bool rmajor = true;  // specify row-major layout
    bool verbose = false;  // specify verbosity in update functions
    // specify 2D qubit arrangement, neighborhood activation level, activator
    unsigned int nrows = 5;
    unsigned int ncols = 5;
    unsigned int rule = 4;
    unsigned int * levels = getLevels(rule, NHOOD);  // utilities/utilities.h
    sprintf(buffer, "Rule: %u", rule);
    printStatusMessage(buffer);  // logging/logging.h
    for (unsigned int i = 0; i < levels[0]; ++i) {
        sprintf(buffer, "valid activation level: %u", levels[i+1]);
        printStatusMessage(buffer);  // logging/logging.h
    }
    unsigned int nqubits = nrows * ncols;
    ComplexMatrix2 activator = getActivator("Hadamard");  // utilities/utilities.h
    // prepare initial condition
    printStatusMessage("Printing initial cond.");  // logging/logging.h
    char * state = initBitString(nqubits);
    setState(state, nrows, ncols, rmajor, "W");  // rules/five-site/utilities.h
    printProductState(state, nrows, ncols, rmajor);  // rules/five-site/logging.h
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    unsigned int base = 2;
    unsigned int index = getStateIndex(state, base);  // utilities/utilities.h
    initClassicalState(qubits, index);  // utilities/utilities.h
    // report execution environment and quantum system
    printStatusMessage("Printing simulated quantum register and runtime info");  // logging/logging.h
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // apply the circuit
    printStatusMessage("Cycling the circuit");  // logging/logging.h
    int outcome = 1;
    printStatusMessage("Initial probability grid");  // logging/logging.h
    printProbabilityGrid(qubits, nrows, ncols, outcome, rmajor);  // rules/five-site/logging.h
    unsigned int ncycles = 1;
    for(unsigned int i = 0; i < ncycles; ++i) {
        updateQubits(qubits, nrows, ncols, levels, activator, rmajor, verbose);  // rules/five-site/five-site.h
        syncQuESTEnv(runtime);
        sprintf(buffer, "Probability grid at cycle: %u", i);
        printStatusMessage(buffer);  // logging/logging.h
        printProbabilityGrid(qubits, nrows, ncols, outcome, rmajor);  // rules/five-site/logging.h
    }
    // cleanup
    printStatusMessage("Finished; cleaning up");  // logging/logging.h
    free(levels);
    free(state);
    destroyQureg(qubits, runtime);
    destroyQuESTEnv(runtime);
    return 0;
}
