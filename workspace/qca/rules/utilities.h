#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../../../QuEST/include/QuEST.h"

#pragma once
#if !defined UTILITIES_H
#define UTILITIES_H

unsigned int getStateIndex(char * bitstring,
                           unsigned int base) {
    return strtoull(bitstring, NULL, base);
}

char * initBitString(unsigned int len) {
    char * out = (char *) calloc(len, sizeof(char));
    for (unsigned int i = 0; i < len; ++i) {
        out[i] = '0';
    }
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

char * bitsFromVal(unsigned int val,
                   unsigned int nbits) {
    char * out = (char *) calloc(nbits, sizeof(char));
    for (unsigned int i = 0; i < nbits; ++i) {
        out[i] = '0';
    }
    unsigned int index = 0;
    for (unsigned int i = 1 << (nbits-1); i > 0; i = i / 2) {
        out[index] = (val & i) ? '1' : '0';
        index = index + 1;
    }
    return out;
}

unsigned int getLevel(char * neighbors,
                      unsigned int nhood) {
    unsigned int out = 0;
    for (unsigned int i = 0; i < nhood-1; ++i) {
        char level = neighbors[i];
        out = out + strtoull(&level, NULL, 10);
    }
    return out;
}

char ** getTotalisticRule(unsigned int level,
                          unsigned int nhood) {
    unsigned int nvalid = nCr(nhood-1, level);
    unsigned int nnhoods = (unsigned int) pow(2, nhood-1);
    char * neighbors = (char *) calloc(nhood-1, sizeof(char));
    char ** out = (char **) calloc(nvalid, sizeof(char *));
    for (unsigned int i = 0; i < nvalid; ++i) {
        out[i] = (char *) calloc(nhood, sizeof(char));
        for (unsigned int j = 0; j < nhood; ++j) {
            out[i][j] = '0';
        }
    }
    unsigned int index = 0;
    unsigned int center = (unsigned int) floor(nhood / 2);
    for (unsigned int i = 0; i < nnhoods; ++i) {
        neighbors = bitsFromVal(i, nhood-1);
        if (getLevel(neighbors, nhood) == level) {
            for (unsigned int j = 0; j < nhood-1; ++j) {
                if (j < center) {
                    out[index][j] = neighbors[j];
                } else {
                    out[index][j+center-1] = neighbors[j];
                }
            }
            index = index + 1;
        }
    }
    free(neighbors);
    return out;
}

unsigned int * getPauliXTargets(char * signature,
                                unsigned int * controls) {
    unsigned int * out = NULL;
    unsigned int nbits = (*signature) / sizeof(char);
    unsigned int ncontrols = (*controls) / sizeof(char);
    if (nbits == ncontrols+1){
        return out;
    }
    unsigned int center = (unsigned int) floor(nbits / 2);
    for (unsigned int i = 0; i < nbits; ++i) {
        char bit = '\0';
        if (i != center) {
            bit = signature[i];
        }
        if (!strcmp(&bit, "0")) {
            unsigned int nout = (*out) / sizeof(unsigned int);
            out = (unsigned int *) realloc(out, nout);
            if (i < center) {
                out[nout-1] = controls[i];
            }
            if (center < i) {
                out[nout-1] = controls[i+center-1];
            }
        }
    }
    return out;
}

void multiPauliX(Qureg qubits,
                 unsigned int * targets) {
    unsigned int ntargets = sizeof(*targets) / sizeof(unsigned int);
    for (unsigned int i = 0; i < ntargets; ++i) {
        pauliX(qubits, targets[i]);
    }
}

void multiControlledActivator(Qureg qubits,
                              unsigned int * controls,
                              unsigned int target,
                              unsigned int level,
                              ComplexMatrix2 activator) {
    unsigned int ncontrols = sizeof(*controls) / sizeof(unsigned int);
    unsigned int nsignatures = nCr(ncontrols, level);
    char ** signatures = getTotalisticRule(level, ncontrols+1);
    unsigned int center = (unsigned int) floor(ncontrols+1 / 2);
    for (unsigned int i = 0; i < nsignatures; ++i) {
        char * signature = signatures[i];
        unsigned int * ptargets = getPauliXTargets(signature, controls);
        multiPauliX(qubits, ptargets);
        multiControlledUnitary(qubits, (int *) controls, ncontrols, target, activator);
        multiPauliX(qubits, ptargets);
        free(ptargets);
    }
    free(signatures);
}

#endif
