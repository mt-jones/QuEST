#include "external/external.h"
#include "rules/five-site/five-site.h"
#include "utilities/utilities.h"

int main(int argc, char * argv[]) {
    char buffer[256];
    if (argc != numArgNames+1) {
        sprintf(buffer, "%s", "Invalid number of command line arguments");
        printStatusMessage(buffer);  // logging/logging.h
        sprintf(buffer, "%s", "Command line arguments must be specified as:");
        printStatusMessage(buffer);  // logging/logging.h
        for (unsigned int i = 0; i < numArgNames; ++i) {
            sprintf(buffer, "%s=...", argNames[i]);
            printStatusMessage(buffer);  // logging/logging.h
        }
        exit(EXIT_FAILURE);
    }
    sprintf(buffer, "%s", "Printing command line inputs");
    printStatusMessage(buffer);  // logging/logging.h
    char * argVals[numArgNames];
    for (int i = 1; i < argc; ++i) {
        char * arg = argv[i];
        char * token = strtok(arg, "=");
        for (unsigned int j = 0; j < numArgNames; ++j) {
            const char * argName = argNames[j];
            if (!strcmp(token, argName)) {
                token = strtok(NULL, "=");
                argVals[j] = token;
            }
        }
    }
    for (unsigned int i = 0; i < numArgNames; ++i) {
        sprintf(buffer, "%s: %s", argNames[i], argVals[i]);
        printStatusMessage(buffer);  // logging/logging.h
    }
    bool rmajor = !strcmp(argVals[0], "true") ? true : false;  // specify row-major layout
    bool verbose = !strcmp(argVals[1], "true") ? true : false;  // specify verbosity in update functions
    // specify 2D qubit arrangement, qubit gate mode, qubit error, neighborhood activation level
    char * endptr = NULL;
    unsigned int inputBase = 10;
    unsigned int nrows = strtoul(argVals[2], &endptr, inputBase);
    unsigned int ncols = strtoul(argVals[3], &endptr, inputBase);
    unsigned int rule = strtoul(argVals[4], &endptr, inputBase);
    unsigned int * levels = getLevels(rule, NHOOD);  // utilities/utilities.h
    sprintf(buffer, "%s", "Printing simulation details");
    printStatusMessage(buffer);  // logging/logging.h
    sprintf(buffer, "Rule: %u", rule);
    printStatusMessage(buffer);  // logging/logging.h
    for (unsigned int i = 0; i < levels[0]; ++i) {
        sprintf(buffer, "valid activation level: %u", levels[i+1]);
        printStatusMessage(buffer);  // logging/logging.h
    }
    unsigned int nqubits = nrows * ncols;
    // prepare initial condition
    unsigned int ic = strtoul(argVals[5], &endptr, inputBase);
    enum qubitGateMode gateMode = (enum qubitGateMode) strtoul(argVals[6], &endptr, inputBase);
    bool openBoundaries = !strcmp(argVals[7], "true") ? true : false;  // specify whether or not to include a boundary of fixed qubits in the circuit
    unsigned int trajectory = strtoul(argVals[8], &endptr, inputBase);
    qreal gateErr = (qreal) strtod(argVals[9], &endptr);
    printStatusMessage("Printing initial cond.");  // logging/logging.h
    char * state = initBitString(nqubits);
    setState(state, nrows, ncols, rmajor, ic);  // rules/five-site/utilities.h
    printProductState(state, nrows, ncols, rmajor);  // rules/five-site/logging.h
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    Qureg workspace = createQureg(nqubits, runtime);
    unsigned int indexBase = 2;
    unsigned int index = getStateIndex(state, indexBase);  // utilities/utilities.h
    initClassicalState(qubits, index);  // utilities/utilities.h
    // report execution environment and quantum system
    printStatusMessage("Printing simulated quantum register and runtime info");  // logging/logging.h
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // prepare data directories
    char directory[128];
    sprintf(directory, "./data/%utraj_%uqubits_%ur_%uc_%uru_%uic_%umode_%4.2ferr", trajectory, nqubits, nrows, ncols, rule, ic, gateMode, gateErr);
    mkdir(directory, S_IRWXU);
    // apply the circuit
    printStatusMessage("Cycling the circuit");  // logging/logging.h
    int outcome = 1;
    if (gateErr >= 0) {
        initRNG();
    } else {
        sprintf(buffer, "%s: %4.2f", "Two qubit gate error must be greater than or equal to zero, but it was set to", gateErr);
        printStatusMessage(buffer);  // logging/logging.h
        exit(EXIT_FAILURE);
    }
    printStatusMessage("Initial probability grid");  // logging/logging.h
    printProbabilityGrid(qubits, nrows, ncols, outcome, rmajor);  // rules/five-site/logging.h
    unsigned int ncycles = strtoul(argVals[10], &endptr, inputBase);
    unsigned int nreduced[] = {1, 2};
    for (unsigned int j = 0; j < sizeof(nreduced) / sizeof(unsigned int); ++j) {
        // sprintf(buffer, "%s/%u_qubit_density_%u_cycle.bin", directory, nreduced[j], 0);
        // void *** density = getQubitDensity(qubits, nqubits, nreduced[j], workspace);  // utilities/utilities.h
        // writeQubitDensity(density, buffer);  // utilities/utilities.h
        // freeQubitDensity(density);  // utilities/utilities.h
    }
    for(unsigned int i = 1; i <= ncycles; ++i) {
        updateQubits(qubits, nrows, ncols, levels, rmajor, verbose, gateMode, openBoundaries, gateErr);  // rules/five-site/five-site.h
        syncQuESTEnv(runtime);
        sprintf(buffer, "Probability grid at cycle: %u", i);
        printStatusMessage(buffer);  // logging/logging.h
        printProbabilityGrid(qubits, nrows, ncols, outcome, rmajor);  // rules/five-site/logging.h
        for (unsigned int j = 0; j < sizeof(nreduced)/sizeof(unsigned int); ++j) {
            // sprintf(buffer, "%s/%u_qubit_density_%u_cycle.bin", directory, nreduced[j], i);
            // void *** density = getQubitDensity(qubits, nqubits, nreduced[j], workspace);  // utilities/utilities.h
            // writeQubitDensity(density, buffer);  // utilities/utilities.h
            // freeQubitDensity(density);  // utilities/utilities.h
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
