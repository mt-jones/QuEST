#include "rules/five-site.h"
#include "../../QuEST/include/QuEST.h"

int main() {
    bool rmajor = true;  // specify row-major layout
    // specify 2D qubit arrangement, neighborhood activation level, activator
    unsigned int nrows = 5;
    unsigned int ncols = 5;
    unsigned int rule = 12;
    unsigned int * levels = getLevels(rule, NHOOD);
    unsigned int nqubits = nrows * ncols;
    ComplexMatrix2 activator = getActivator("hadamard");
    // prepare initial condition
    char * state = initBitString(nqubits);
    setState(state, nrows, ncols, rmajor, "W");
    printStatusMessage("Printing initial cond.");
    printProductState(state, nrows, ncols, rmajor);
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    initClassicalState(qubits, getStateIndex(state, 2));
    // report execution environment and quantum system
    printStatusMessage("Printing simulated quantum register and runtime info");
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // apply the circuit
    unsigned int ncycles = 1;
    printStatusMessage("Cycling the circuit");
    char buffer[80];
    for(unsigned int i = 0; i < ncycles; ++i) {
        sprintf(buffer, "cycle: %u", i);
        printStatusMessage(buffer);
        updateQubits(qubits, nrows, ncols, levels, activator, rmajor);
    }
    syncQuESTEnv(runtime);
    // cleanup
    printStatusMessage("Finished; cleaning up");
    free(state);
    destroyQureg(qubits, runtime);
    destroyQuESTEnv(runtime);
    return 0;
}