#include "../external/external.h"
#include "../logging/logging.h"

#pragma once
#if !defined UTILITIES_H
#define UTILITIES_H
#define INVALID UINT_MAX

void initRNG() {
    srand(time(NULL));
}

double rng(double rmin,
           double rmax) {
    double range = rmax - rmin;
    double out = rmin + ((double) rand() / ((double) RAND_MAX)) * range;
    return out;
}

unsigned int roll(unsigned int * vals,
                  unsigned int nvals) {
    unsigned int out = 0;
    double rmin = 0;
    double rmax = nvals;
    double value = rng(rmin, rmax);
    unsigned int index = 0;
    for (unsigned int i = 0; i < nvals; ++i) {
        if (i <= value && value < i + 1) {
            index = i;
        }
    }
    out = vals[index];
    return out;
}

void swap(char * a,
          char * b) {
    char c = *a;
    *a = *b;
    *b = c;
}

void reverse(char * buffer,
             unsigned int len) {
    for (unsigned int i = 0; i < len; ++i) {
        unsigned int j = len - (i + 1);
        if (i < j) {
            swap(&buffer[i], &buffer[j]);
        }
    }
}

char * getValueBaseN(unsigned int val,
                     unsigned int base,
                     unsigned int nbits) {
    unsigned int minbase = 2;
    unsigned int maxbase = 32;
    unsigned int hoffset = 10;
    char * out = (char *) calloc(nbits+1, sizeof(char));
    for (unsigned int i = 0; i < nbits; ++i) {
        out[i] = '0';
    }
	if (minbase <= base && base <= maxbase) {
        unsigned int index = 0;
        for (unsigned int v = val; v > 0; v = v/base) {
            if (index < nbits) {
                unsigned int rem = v % base;
                out[index] = (rem >= hoffset) ? 'A' + (rem - hoffset) : '0' + rem;
            }
            index = index + 1;
        }
    }
    reverse(out, nbits);
    return out;
}

unsigned int getValueBase10(char val) {
    unsigned int out = val;
	unsigned int hoffset = 10;
    out = ('A' <= val) ? out - 'A' + hoffset : out - '0';
    return out;
}

unsigned int * appendArray(unsigned int * arr,
                           unsigned int * vals,
                           unsigned int nvals) {
    unsigned int len = 0;
    if (arr != NULL) {
        len = arr[0];
    }
    arr = (unsigned int *) realloc(arr, (len+nvals+1)*sizeof(unsigned int));
    for (unsigned int i = 0; i < nvals; ++i) {
        arr[len+i+1] = vals[i];
    }
    arr[0] = len + nvals;
    return arr;
}

unsigned int * initArray() {
    unsigned int nout = 0, nvals = 0;
    unsigned int * out = NULL;
    out = appendArray(out, &nout, nvals);
    return out;
}

unsigned int nCr(unsigned int n,
                 unsigned int r) {
    if (n < r) {
        return 0;
    }
    if (n < 2 * r) {
        r = n - r;
    }
    if (r == 0) {
        return 1;
    }
    unsigned int result = n;
    for(unsigned int i = 2; i <= r; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}

unsigned int * getValidValues(unsigned int * arr) {
    unsigned int * out = initArray();
    unsigned int val, nvals = 1;
    for (unsigned int i = 0; i < arr[0]; ++i) {
        unsigned int val = arr[i+1];
        if (val != INVALID) {
            out = appendArray(out, &val, nvals);
        }
    }
    return out;
}

unsigned int getStateIndex(char * bitstring,
                           unsigned int base) {
    unsigned int out;
    char * endptr = NULL;
    out = strtoul(bitstring, &endptr, base);
    return out;
}

char * initBitString(unsigned int len) {
    char * out = (char *) calloc(len+1, sizeof(char));
    for (unsigned int i = 0; i < len; ++i) {
        out[i] = '0';
    }
    out[len] = '\0';
    return out;
}

unsigned int getLevel(char * neighbors,
                      unsigned int nhood) {
    unsigned int out = 0, base = 10;
    for (unsigned int i = 0; i < nhood-1; ++i) {
        char level = neighbors[i];
        out = out + strtoull(&level, NULL, base);
    }
    return out;
}

unsigned int * getLevels(unsigned int rule,
                         unsigned int nhood) {
    unsigned int base = 2;
    unsigned int val, nvals = 1;
    unsigned int * out = initArray();
    char * binrule = getValueBaseN(rule, base, nhood);
    for (unsigned int i = 0; i < nhood; ++i) {
        char bit = binrule[i];
        val = (nhood - 1) - i;
        if (bit == '1') {
            out = appendArray(out, &val, nvals);
        }
    }
    free(binrule);
    return out;
}

char ** getTotalisticRule(unsigned int level,
                          unsigned int nhood) {
    unsigned int nvalid = nCr(nhood-1, level);
    unsigned int nnhoods = (unsigned int) pow(2, nhood-1);
    char ** out = (char **) calloc(nvalid, sizeof(char *));
    for (unsigned int i = 0; i < nvalid; ++i) {
        out[i] = (char *) calloc(nhood, sizeof(char));  // last character is null terminator
        for (unsigned int j = 0; j < nhood-1; ++j) {
            out[i][j] = '0';
        }
    }
    unsigned int base = 2;
    unsigned int index = 0;
    for (unsigned int i = 0; i < nnhoods; ++i) {
        char * neighbors = getValueBaseN(i, base, nhood-1);
        unsigned int activation = getLevel(neighbors, nhood);
        if (activation == level) {
            for (unsigned int j = 0; j < nhood-1; ++j) {
                out[index][j] = neighbors[j];
            }
            index = index + 1;
        }
        free(neighbors);
    }
    return out;
}

unsigned int getTarget(unsigned int * neighborhood) {
    unsigned int center = (unsigned int) floor(neighborhood[0] / 2);
    unsigned int out = neighborhood[center+1];
    return out;
}

unsigned int * getControls(unsigned int * neighborhood) {
    unsigned int * out = initArray();
    unsigned int val, nvals = 1;
    unsigned int center = (unsigned int) floor(neighborhood[0] / 2);
    for (unsigned int i = 0; i < neighborhood[0]; ++i) {
        if (i != center) {
            val = neighborhood[i+1];
            out = appendArray(out, &val, nvals);
        }
    }
    return out;
}

unsigned int * getPauliXTargets(char * signature,
                                unsigned int * controls) {
    unsigned int * out = initArray();
    unsigned int val, nvals = 1;
    for (unsigned int i = 0; i < controls[0]; ++i) {
        val = INVALID;
        char bit = signature[i];
        if (bit == '0') {
            val = controls[i+1];
            if (val != INVALID) {
                out = appendArray(out, &val, nvals);
            }
        }
    }
    return out;
}

void pauliN(Qureg qubits,
            unsigned int pcode,
            unsigned int target) {
    if (pcode == 1) {
        pauliX(qubits, (int) target);
    } else if (pcode == 2) {
        pauliY(qubits, (int) target);
    } else if (pcode == 3) {
        pauliZ(qubits, (int) target);
    }
}

void multiPauliX(Qureg qubits,
                 unsigned int * targets) {
    for (unsigned int i = 0; i < targets[0]; ++i) {
        pauliX(qubits, targets[i+1]);
    }
}

ComplexMatrix2 getActivator(unsigned int nneighbors) {
    ComplexMatrix2 activator;
    double revals[4];
    double imvals[4];
    if (nneighbors == 2) {
        // hadamard
        printf("CASE 2 \n");
        revals[0] = 0.70710678118654752440084436210485;
        revals[1] = 0.70710678118654752440084436210485;
        revals[2] = 0.70710678118654752440084436210485;
        revals[3] = -0.70710678118654752440084436210485;
        imvals[0] = 0;
        imvals[1] = 0;
        imvals[2] = 0;
        imvals[3] = 0;
    } else if (nneighbors == 3) {
        // two-thirds-root hadamard
        printf("CASE 3 \n");
        revals[0] = 0.78033008588991064330063327157864;
        revals[1] = 0.53033008588991064330063327157864;
        revals[2] = 0.53033008588991064330063327157864;
        revals[3] = -0.28033008588991064330063327157864;
        imvals[0] = 0.12682648404432206110720107603823;
        imvals[1] = -0.30618621784789726227466050933824;
        imvals[2] = -0.30618621784789726227466050933824;
        imvals[3] = 0.73919891974011658565652209471470;
    } else if (nneighbors == 4) {
        // square-root hadamard
        printf("CASE 4 \n");
        revals[0] = 0.85355339059327376220042218105242;
        revals[1] = 0.35355339059327376220042218105242;
        revals[2] = 0.35355339059327376220042218105242;
        revals[3] = 0.14644660940672623779957781894758;
        imvals[0] = 0.14644660940672623779957781894758;
        imvals[1] = -0.35355339059327376220042218105242;
        imvals[2] = -0.35355339059327376220042218105242;
        imvals[3] = 0.85355339059327376220042218105242;
    }
    activator = (ComplexMatrix2) {
        .real = {{revals[0], revals[1]},
                 {revals[2], revals[3]}},
        .imag = {{imvals[0], imvals[1]},
                 {imvals[2], imvals[3]}}
    };
    return activator;
}

enum qubitGateMode {
    twoQubitGates = 2,
    fiveQubitGates = 5
};

void multiControlledActivator(Qureg qubits,
                              unsigned int * controls,
                              unsigned int target,
                              unsigned int level,
                              enum qubitGateMode mode,
                              qreal qubitGateErr) {
    unsigned int * vcontrols = getValidValues(controls);
    ComplexMatrix2 activator = getActivator(vcontrols[0]);
    unsigned int nsignatures = nCr(vcontrols[0], level);
    char ** signatures = getTotalisticRule(level, vcontrols[0]+1);
    for (unsigned int i = 0; i < nsignatures; ++i) {
        char * signature = signatures[i];
        printf("%s\n", signature);
        unsigned int * ptargets = getPauliXTargets(signature, vcontrols);
        multiPauliX(qubits, ptargets);
        if (mode == twoQubitGates) {
            for (unsigned int j = 0; j < vcontrols[0]; ++j) {
                controlledUnitary(qubits, vcontrols[j+1], target, activator);
                if (qubitGateErr > 0) {
                    double rmin = 0;
                    double rmax = 100;
                    double qubitGateErrRoll = rng(rmin, rmax);
                    bool qubitGateErrQ = (qubitGateErrRoll <= qubitGateErr) ? true : false;
                    if (qubitGateErrQ) {
                        unsigned int validPauliCodes[] = {1, 2, 3};
                        unsigned int npcodes = sizeof(validPauliCodes) / sizeof(unsigned int);
                        unsigned int pauliCode = roll(validPauliCodes, npcodes);
                        pauliN(qubits, pauliCode, target);
                    }
                }
            }
        } else if (mode == fiveQubitGates) {
            multiControlledUnitary(qubits, (int *) &vcontrols[1], vcontrols[0], target, activator);
        } else {
            char buffer[256];
            sprintf(buffer, "%s: %u", "Specified qubit gate mode is not supported", mode);
            printStatusMessage(buffer);
            exit(EXIT_FAILURE);
        }
        multiPauliX(qubits, ptargets);
        free(ptargets);
    }
    free(vcontrols);
    for (unsigned int i = 0; i < nsignatures; ++i) {
        free(signatures[i]);
    }
    free(signatures);
}

char ** getPauliSequence(unsigned int nreduced) {
    unsigned int npauli = 4;
    unsigned int base = 10, nbits = 8;
    unsigned int nout = (unsigned int) pow(npauli, nreduced);
    char ** out = (char **) calloc(nout+1, sizeof(char *));
    out[0] = getValueBaseN(nout, base, nbits);
    for (unsigned int i = 0; i < nout; ++i) {
        out[i+1] = getValueBaseN(i, npauli, nreduced);
    }
    return out;
}

char ** getQubitDensitySequence(unsigned int nqubits,
                                unsigned int nreduced) {
    unsigned int nvals = (unsigned int) pow(nqubits, nreduced);
    unsigned int nout = 1;
    char ** out = (char **) calloc(nout, sizeof(char *));
    for (unsigned int i = 0; i < nvals; ++i) {
        bool unique = true;
        char * bitstring = getValueBaseN(i, nqubits, nreduced);
        for (unsigned int j = 0; j < nreduced; ++j) {
            char jbit = bitstring[j];
            unsigned int jvalue = getValueBase10(jbit);
            for (unsigned int k = j+1; k < nreduced; ++k) {
                char kbit = bitstring[k];
                unsigned int kvalue = getValueBase10(kbit);
                if (jvalue == kvalue) {
                    unique = false;
                    free(bitstring);
                    break;
                }
            }
            if (!unique) {
                break;
            }
        }
        if (unique) {
            nout = nout + 1;
            out = (char **) realloc(out, nout*sizeof(char *));
            out[nout-1] = bitstring;
        }
    }
    unsigned int base = 10, nbits = 8;
    out[0] = getValueBaseN(nout-1, base, nbits);
    return out;
}

void *** getQubitDensity(Qureg qubits,
                         unsigned int nqubits,
                         unsigned int nreduced,
                         Qureg workspace) {
    unsigned int nout = 3;
    unsigned int qindex = 0;
    unsigned int pindex = 1;
    unsigned int cindex = 2;
    void *** out = (void ***) calloc(nout, sizeof(void **));
    char * endptr = NULL;
    unsigned int base = 10;
    char ** psequence = getPauliSequence(nreduced);
    unsigned int npsequence = strtoll(psequence[0], &endptr, base);
    unsigned int ** pseq = (unsigned int **) calloc(npsequence+1, sizeof(unsigned int *));
    for (unsigned int i = 0; i < npsequence+1; ++i) {
        unsigned int val, nvals = 1;
        pseq[i] = initArray();
        if (i == 0) {
            pseq[i] = appendArray(pseq[i], &npsequence, nvals);
            continue;
        }
        char * bitstring = psequence[i];
        for (unsigned int j = 0; j < nreduced; ++j) {
            char bit = bitstring[j];
            val = getValueBase10(bit);
            pseq[i] = appendArray(pseq[i], &val, nvals);
        }
    }
    char ** qdsequence = getQubitDensitySequence(nqubits, nreduced);
    unsigned int nqdsequence = strtoll(qdsequence[0], &endptr, base);
    unsigned int ** qseq = (unsigned int **) calloc(nqdsequence+1, sizeof(unsigned int *));
    for (unsigned int i = 0; i < nqdsequence+1; ++i) {
        unsigned int val, nvals = 1;
        qseq[i] = initArray();
        if (i == 0) {
            qseq[i] = appendArray(qseq[i], &nqdsequence, nvals);
            continue;
        }
        char * bitstring = qdsequence[i];
        for (unsigned int j = 0; j < nreduced; ++j) {
            char bit = bitstring[j];
            val = getValueBase10(bit);
            qseq[i] = appendArray(qseq[i], &val, nvals);
        }
    }
    unsigned int ** qindices = (unsigned int **) calloc(nqdsequence*npsequence+1, sizeof(unsigned int *));
    unsigned int ** pindices = (unsigned int **) calloc(nqdsequence*npsequence+1, sizeof(unsigned int *));
    qreal ** coeffs = (qreal **) calloc(nqdsequence*npsequence+1, sizeof(qreal *));
    unsigned int nvals = 1;
    unsigned int length = nqdsequence * npsequence;
    qindices[0] = (unsigned int *) calloc(nvals+1, sizeof(unsigned int));
    qindices[0][0] = nvals;
    qindices[0][1] = length;
    pindices[0] = (unsigned int *) calloc(nvals+1, sizeof(unsigned int));
    pindices[0][0] = nvals;
    pindices[0][1] = length;
    coeffs[0] = (qreal *) calloc(nvals+1, sizeof(qreal));
    coeffs[0][0] = nvals;
    coeffs[0][1] = length;
    unsigned int qlength = (qseq[0][0] >= 1) ? qseq[0][1] : qseq[0][0];
    unsigned int plength = (pseq[0][0] >= 1) ? pseq[0][1] : pseq[0][0];
    for (unsigned int i = 0; i < qlength; ++i) {
        unsigned int * targets = qseq[i+1];
        for (unsigned int j = 0; j < plength; ++j) {
            unsigned int * pcodes = pseq[j+1];
            qindices[i*npsequence+j+1] = (unsigned int *) calloc(targets[0]+1, sizeof(unsigned int));
            for (unsigned int k = 0; k < targets[0]+1; ++k) {
                qindices[i*npsequence+j+1][k] = targets[k];
            }
            pindices[i*npsequence+j+1] = (unsigned int *) calloc(pcodes[0]+1, sizeof(unsigned int));
            for (unsigned int k = 0; k < pcodes[0]+1; ++k) {
                pindices[i*npsequence+j+1][k] = pcodes[k];
            }
            coeffs[i*npsequence+j+1] = (qreal *) calloc(nvals+1, sizeof(qreal));
            coeffs[i*npsequence+j+1][0] = nvals;
            coeffs[i*npsequence+j+1][1] = calcExpecPauliProd(qubits, (int *) &targets[1], (enum pauliOpType *) &pcodes[1], nreduced, workspace);
        }
    }
    out[qindex] = (void **) qindices;
    out[pindex] = (void **) pindices;
    out[cindex] = (void **) coeffs;
    for (unsigned int i = 0; i < npsequence+1; ++i) {
        free(psequence[i]);
        free(pseq[i]);
    }
    free(psequence);
    free(pseq);
    for (unsigned int i = 0; i < nqdsequence+1; ++i) {
        free(qdsequence[i]);
        free(qseq[i]);
    }
    free(qdsequence);
    free(qseq);
    return out;
}

void writeQubitDensity(void *** density,
                       char * fname) {
    FILE * fptr = fopen(fname, "wb");
    unsigned int qindex = 0;
    unsigned int pindex = 1;
    unsigned int cindex = 2;
    unsigned int ** qindices = (unsigned int **) density[qindex];
    unsigned int ** pindices = (unsigned int **) density[pindex];
    qreal ** coeffs = (qreal **) density[cindex];
    unsigned int qlength = (qindices[0][0] >= 1) ? qindices[0][1] : qindices[0][0];
    unsigned int plength = (pindices[0][0] >= 1) ? pindices[0][1] : pindices[0][0];
    unsigned int clength = (coeffs[0][0] >= 1) ? coeffs[0][1] : coeffs[0][0];
    unsigned int nvals = 1;
    fwrite(&qlength, sizeof(unsigned int), nvals, fptr);
    fwrite(&plength, sizeof(unsigned int), nvals, fptr);
    fwrite(&clength, sizeof(unsigned int), nvals, fptr);
    for (unsigned int i = 0; i < qlength; ++i) {
        unsigned int nvals = 1;
        unsigned int * vals = qindices[i+1];
        for (unsigned int j = 0; j < vals[0]+1; ++j) {
            unsigned int val = vals[j];
            fwrite(&val, sizeof(unsigned int), nvals, fptr);
        }
    }
    for (unsigned int i = 0; i < plength; ++i) {
        unsigned int nvals = 1;
        unsigned int * vals = pindices[i+1];
        for (unsigned int j = 0; j < vals[0]+1; ++j) {
            unsigned int val = vals[j];
            fwrite(&val, sizeof(unsigned int), nvals, fptr);
        }
    }
    for (unsigned int i = 0; i < clength; ++i) {
        unsigned int nvals = 1;
        qreal * vals = coeffs[i+1];
        for (unsigned int j = 0; j < vals[0]+1; ++j) {
            qreal val = vals[j];
            fwrite(&val, sizeof(qreal), nvals, fptr);
        }
    }
    fclose(fptr);
}

void freeQubitDensity(void *** density) {
    unsigned int qindex = 0;
    unsigned int pindex = 1;
    unsigned int cindex = 2;
    unsigned int ** qindices = (unsigned int **) density[qindex];
    unsigned int ** pindices = (unsigned int **) density[pindex];
    qreal ** coeffs = (qreal **) density[cindex];
    unsigned int qlength = (qindices[0][0] >= 1) ? qindices[0][1] : qindices[0][0];
    unsigned int plength = (pindices[0][0] >= 1) ? pindices[0][1] : pindices[0][0];
    unsigned int clength = (coeffs[0][0] >= 1) ? coeffs[0][1] : coeffs[0][0];
    for (unsigned int i = 0; i < qlength+1; ++i) {
        free(qindices[i]);
    }
    free(qindices);
    for (unsigned int i = 0; i < plength+1; ++i) {
        free(pindices[i]);
    }
    free(pindices);
    for (unsigned int i = 0; i < clength+1; ++i) {
        free(coeffs[i]);
    }
    free(coeffs);
    free(density);
}

#endif
