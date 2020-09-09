#include "external/external.h"
#include "rules/five-site/five-site.h"

int main() {
    char buffer[256];
    bool rmajor = true;  // specify row-major layout
    bool verbose = false;  // specify verbosity in update functions
    // specify 2D qubit arrangement, neighborhood activation level, activator
    unsigned int nrows = 3;
    unsigned int ncols = 3;
    unsigned int rule = 12;
    unsigned int * levels = getLevels(rule, NHOOD);  // utilities/utilities.h
    sprintf(buffer, "Rule: %u", rule);
    printStatusMessage(buffer);  // logging/logging.h
    for (unsigned int i = 0; i < levels[0]; ++i) {
        sprintf(buffer, "valid activation level: %u", levels[i+1]);
        printStatusMessage(buffer);  // logging/logging.h
    }
    unsigned int nqubits = nrows * ncols;
    ComplexMatrix2 activator = getActivator("Sqrt-Hadamard");  // utilities/utilities.h
    // prepare initial condition
    char * ic = "W";
    printStatusMessage("Printing initial cond.");  // logging/logging.h
    char * state = initBitString(nqubits);
    setState(state, nrows, ncols, rmajor, ic);  // rules/five-site/utilities.h
    printProductState(state, nrows, ncols, rmajor);  // rules/five-site/logging.h
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    Qureg workspace = createQureg(nqubits, runtime);
    unsigned int base = 2;
    unsigned int index = getStateIndex(state, base);  // utilities/utilities.h
    initClassicalState(qubits, index);  // utilities/utilities.h
    // report execution environment and quantum system
    printStatusMessage("Printing simulated quantum register and runtime info");  // logging/logging.h
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // prepare directories data
    char directory[128];
    sprintf(directory, "./data/%uqubits_%ur_%uc_%uru_%sic", nqubits, nrows, ncols, rule, ic);
    mkdir(directory, S_IRWXU);
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
        unsigned int nreduced[] = {1, 2};
        for (unsigned int j = 0; j < sizeof(nreduced)/sizeof(unsigned int); ++j) {
            sprintf(buffer, "%s/%u_qubit_density_%u_cycle.bin", directory, nreduced[j], i);
            void *** density = getQubitDensity(qubits, nqubits, nreduced[j], workspace);  // utilities/utilities.h
            writeQubitDensity(density, buffer);  // utilities/utilities.h
            freeQubitDensity(density);  // utilities/utilities.h
        }
    }
    // cleanup
    printStatusMessage("Finished; cleaning up");  // logging/logging.h
    free(levels);
    free(state);
    destroyQureg(qubits, runtime);
    destroyQureg(workspace, runtime);
    destroyQuESTEnv(runtime);
    return 0;
}
