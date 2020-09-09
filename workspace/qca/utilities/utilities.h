#include "../external/external.h"
#include <bits/types/FILE.h>

#pragma once
#if !defined UTILITIES_H
#define UTILITIES_H
#define INVALID UINT_MAX

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
    out = ('A' <= val) ? out - 'A' : out - '0';
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
        out[i] = (char *) calloc(nhood, sizeof(char));
        for (unsigned int j = 0; j < nhood; ++j) {
            out[i][j] = '0';
        }
    }
    unsigned int base = 2;
    unsigned int index = 0;
    unsigned int center = (unsigned int) floor(nhood / 2);
    for (unsigned int i = 0; i < nnhoods; ++i) {
        char * neighbors = getValueBaseN(i, base, nhood-1);
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
                                unsigned int * controls,
                                unsigned int nhood) {
    unsigned int * out = initArray();
    unsigned int val, nvals = 1;
    unsigned int center = (unsigned int) floor(nhood / 2);
    for (unsigned int i = 0; i < nhood; ++i) {
        val = INVALID;
        char bit = '\0';
        if (i != center) {
            bit = signature[i];
        }
        if (bit == '0') {
            if (i < center) {
                val = controls[i+1];
            }
            if (center < i) {
                val = controls[i];
            }
            if (val != INVALID) {
                out = appendArray(out, &val, nvals);
            }
        }
    }
    return out;
}

void multiPauliX(Qureg qubits,
                 unsigned int * targets) {
    for (unsigned int i = 0; i < targets[0]; ++i) {
        pauliX(qubits, targets[i+1]);
    }
}

ComplexMatrix2 getActivator(char * name) {
    ComplexMatrix2 activator;
    if (!strcmp(name, "Hadamard")) {
        double reval[4] = {1/sqrt(2), 1/sqrt(2),
                          1/sqrt(2), -1/sqrt(2)};
        activator = (ComplexMatrix2) {
            .real={{reval[0], reval[1]},
                   {reval[2], reval[3]}},
            .imag={{0, 0},
                   {0, 0}}
        };
    } else if (!strcmp(name, "Sqrt-Hadamard")) {
        double reval[4] = {0.25 * (2 + sqrt(2)), 0.5 * 1/sqrt(2),
                           0.5 * 1/sqrt(2), 0.25 * (2 - sqrt(2))};
        double imval[4] = {0.25 * (2 - sqrt(2)), -0.5 * 1/sqrt(2),
                           -0.5 * 1/sqrt(2), 0.25 * (2 + sqrt(2))};
        activator = (ComplexMatrix2) {
            .real={{reval[0], reval[1]},
                   {reval[2], reval[3]}},
            .imag={{imval[0], imval[1]},
                   {imval[2], imval[3]}}
        };
    } else if (!strcmp(name, "Pauli-X")) {
        double reval[4] = {0, 1,
                           1, 0};
        activator = (ComplexMatrix2) {
            .real={{reval[0], reval[1]},
                   {reval[2], reval[3]}},
            .imag={{0, 0},
                   {0, 0}}
        };
    }
    return activator;
}

void multiControlledActivator(Qureg qubits,
                              unsigned int * controls,
                              unsigned int nhood,
                              unsigned int target,
                              unsigned int level,
                              ComplexMatrix2 activator) {
    unsigned int nsignatures = nCr(controls[0], level);
    char ** signatures = getTotalisticRule(level, nhood);
    unsigned int * vcontrols = getValidValues(controls);
    for (unsigned int i = 0; i < nsignatures; ++i) {
        char * signature = signatures[i];
        unsigned int * ptargets = getPauliXTargets(signature, controls, nhood);
        multiPauliX(qubits, ptargets);
        multiControlledUnitary(qubits, (int *) &vcontrols[1], vcontrols[0], target, activator);
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
            coeffs[i*npsequence+j+1][1] = calcExpecPauliProd(qubits, (int *) &targets[1], (enum pauliOpType *) pcodes, nreduced, workspace);
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
