#include "rules/five-site.h"
#include "../../QuEST/include/QuEST.h"

int main() {
    bool rmajor = true;  // specify row-major layout
    // specify 2D qubit arrangement
    unsigned int nrows = 5;
    unsigned int ncols = 5;
    unsigned int nqubits = nrows * ncols;
    // prepare initial condition
    char * state = initBitString(nqubits);
    setState(state, nrows, ncols, rmajor, "W");
    // prepare runtime environment
    QuESTEnv runtime = createQuESTEnv();
    // prepare qubit system with initial condition
    Qureg qubits = createQureg(nqubits, runtime);
    initClassicalState(qubits, getStateIndex(state, 2));
    // report execution environment and quantum system
    reportQuregParams(qubits);
    reportQuESTEnv(runtime);
    // apply the circuit
    unsigned int ncycles = 100;
    for(int i=0; i<ncycles; ++i) {
        updateQubits();
    }
    // cleanup
    destroyQureg(qubits, runtime);
    destroyQuESTEnv(runtime);
    return 0;
}