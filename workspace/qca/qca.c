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
    printf("--------------------------\n");
    printf("| Printing initial cond. |\n");
    printf("--------------------------\n");
    printState(state, nrows, ncols, rmajor);
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    initClassicalState(qubits, getStateIndex(state, 2));
    // report execution environment and quantum system
    printf("--------------------------------------------\n");
    printf("| Printing simulated quantum register info |\n");
    printf("--------------------------------------------\n");
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // apply the circuit
    unsigned int ncycles = 1;
    printf("------------------------------------\n");
    printf("| Applying cycles onto the circuit |\n");
    printf("------------------------------------\n");
    for(int i = 0; i < ncycles; ++i) {
        updateQubits(qubits, nrows, ncols, levels, activator, rmajor);
    }
    // cleanup
    free(state);
    destroyQureg(qubits, runtime);
    destroyQuESTEnv(runtime);
    return 0;
}