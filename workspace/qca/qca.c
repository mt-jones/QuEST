#include "rules/five-site.h"
#include "../../QuEST/include/QuEST.h"

int main() {
    char buffer[80];
    bool rmajor = true;  // specify row-major layout
    // specify 2D qubit arrangement, neighborhood activation level, activator
    unsigned int nrows = 5;
    unsigned int ncols = 5;
    unsigned int rule = 20;
    unsigned int * levels = getLevels(rule, NHOOD);
    sprintf(buffer, "Rule: %u", rule);
    printStatusMessage(buffer);
    for (unsigned int i = 0; i < levels[0]; ++i) {
        sprintf(buffer, "valid activation level: %u", levels[i+1]);
        printStatusMessage(buffer);
    }
    unsigned int nqubits = nrows * ncols;
    ComplexMatrix2 activator = getActivator("Hadamard");
    // prepare initial condition
    printStatusMessage("Printing initial cond.");
    char * state = initBitString(nqubits);
    setState(state, nrows, ncols, rmajor, "W");
    printProductState(state, nrows, ncols, rmajor);
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    unsigned int base = 2;
    unsigned int index = getStateIndex(state, base);
    initClassicalState(qubits, index);
    // report execution environment and quantum system
    printStatusMessage("Printing simulated quantum register and runtime info");
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // apply the circuit
    printStatusMessage("Cycling the circuit");
    int outcome = 1;
    printStatusMessage("Initial probability grid");
    printProbabilityGrid(qubits, nrows, ncols, outcome, rmajor);
    unsigned int ncycles = 0;
    for(unsigned int i = 0; i < ncycles; ++i) {
        updateQubits(qubits, nrows, ncols, levels, activator, rmajor);
        syncQuESTEnv(runtime);
        sprintf(buffer, "Probability grid at cycle: %u", i);
        printStatusMessage(buffer);
        printProbabilityGrid(qubits, nrows, ncols, outcome, rmajor);
    }
    // cleanup
    printStatusMessage("Finished; cleaning up");
    free(levels);
    free(state);
    destroyQureg(qubits, runtime);
    destroyQuESTEnv(runtime);
    return 0;
}
