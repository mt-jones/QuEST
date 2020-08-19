#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>


#include "utilities.h"
#include "../../../QuEST/include/QuEST.h"

#pragma once
#if !defined FIVE_SITE_H
#define FIVE_SITE_H
#define NHOOD 5

unsigned int get1dIndex(unsigned int row,
                        unsigned int col,
                        unsigned int nrows,
                        unsigned int ncols,
                        bool rmajor) {
    unsigned int out;
    if (rmajor) {
        out = row * ncols + col;
    } else {
        out = col * nrows + row;
    }
    return out;
}

unsigned int * get2dIndex(unsigned int index,
                          unsigned int nrows,
                          unsigned int ncols,
                          bool rmajor) {
    unsigned int * out = initArray();
    if (rmajor) {
        unsigned int count = 0;
        for (unsigned int i = 0; i < nrows; ++i) {
            for (unsigned int j = 0; j < ncols; ++j) {
                if (count == index) {
                    unsigned int val[] = {i, j};
                    unsigned int nvals = sizeof(val) / sizeof(unsigned int);
                    out = appendArray(out, val, nvals);
                }
                count = count + 1;
            }
        }
    } else {
        unsigned int count = 0;
        for (unsigned int j = 0; j < ncols; ++j) {
            for (unsigned int i = 0; i < nrows; ++i) {
                if (count == index) {
                    unsigned int val[] = {i, j};
                    unsigned int nvals = sizeof(val) / sizeof(unsigned int);
                    out = appendArray(out, val, nvals);
                }
                count = count + 1;
            }
        }
    }
    return out;
}

bool indexIsValid(unsigned int index,
                  unsigned int nrows,
                  unsigned int ncols) {
    bool out = true;
    if (0 > index || index >= nrows * ncols) {
        out = false;
    }
    return out;
}

void setState(char * bitstring,
              unsigned int nrows,
              unsigned int ncols,
              bool rmajor,
              char * state) {
    if (!strcmp(state, "W")) {
        unsigned int row = (unsigned int) floor(nrows / 2);
        unsigned int col = (unsigned int) floor(ncols / 2);
        unsigned int up = get1dIndex(row+1, col, nrows, ncols, rmajor);
        unsigned int down = get1dIndex(row-1, col, nrows, ncols, rmajor);
        unsigned int center = get1dIndex(row, col, nrows, ncols, rmajor);
        unsigned int left = get1dIndex(row, col-1, nrows, ncols, rmajor);
        unsigned int right = get1dIndex(row, col+1, nrows, ncols, rmajor);
        if (indexIsValid(up, nrows, ncols)) {
            bitstring[up] = '1';
        }
        if (indexIsValid(down, nrows, ncols)) {
            bitstring[down] = '1';
        }
        if (indexIsValid(left, nrows, ncols)) {
            bitstring[left] = '1';
        }
        if (indexIsValid(right, nrows, ncols)) {
            bitstring[right] = '1';
        }
    }
}

void printState(char * state,
                unsigned int nrows,
                unsigned int ncols,
                bool rmajor) {
    for (unsigned int j = 0; j < ncols; ++j) {
        for (unsigned int i = 0; i < nrows; ++i) {
            unsigned int index = get1dIndex(i, j, nrows, ncols, rmajor);
            printf("%c ", state[index]);
        }
        printf("\n");
    }
}

void printNeighborhood(unsigned int * neighborhood,
                       unsigned int nrows,
                       unsigned int ncols,
                       bool rmajor) {
    unsigned int cindex = (unsigned int) floor(neighborhood[0] / 2);
    unsigned int index = neighborhood[cindex+1];
    unsigned int * center = get2dIndex(index, nrows, ncols, rmajor);
    char buffer[80];
    sprintf(buffer, "| c: %u; (%u, %u) | ", index, center[1], center[2]);
    printf("%s", buffer);
    for (unsigned int i = 0; i < neighborhood[0]; ++i) {
        if (i+1 != cindex+1) {
            unsigned int val = neighborhood[i+1];
            if(val != INVALID) {
                unsigned int * control = get2dIndex(val, nrows, ncols, rmajor);
                sprintf(buffer, "ctrl: %u; (%u, %u) | ", val, control[1], control[2]);
                printf("%s", buffer);
                free(control);
            }
        }
    }
    printf("\n");
    free(center);
}

unsigned int * getNeighborhood(unsigned int row,
                               unsigned int col,
                               unsigned int nrows,
                               unsigned int ncols,
                               bool rmajor) {
    unsigned int * out = initArray();
    unsigned int up = get1dIndex(row, col-1, nrows, ncols, rmajor);
    unsigned int down = get1dIndex(row, col+1, nrows, ncols, rmajor);
    unsigned int center = get1dIndex(row, col, nrows, ncols, rmajor);
    unsigned int left = get1dIndex(row-1, col, nrows, ncols, rmajor);
    unsigned int right = get1dIndex(row+1, col, nrows, ncols, rmajor);
    unsigned int neighborhood[] = {up, down, center, left, right};
    for (unsigned int i = 0; i < sizeof(neighborhood) / sizeof(unsigned int); ++i) {
        if (0 > neighborhood[i] || neighborhood[i] >= nrows * ncols) {
            neighborhood[i] = INVALID;
        }
    }
    out = appendArray(out, neighborhood, sizeof(neighborhood)/sizeof(unsigned int));
    return out;
}

void updateQubits(Qureg qubits,
                  unsigned int nrows,
                  unsigned int ncols,
                  unsigned int * levels,
                  ComplexMatrix2 activator,
                  bool rmajor) {
    unsigned int nlevels = levels[0];
    for (unsigned int offset = 0; offset < 2; ++offset) {
        for (unsigned int i = offset; i < nrows; i = i+2) {
            for (unsigned int j = offset; j < ncols; j = j+2) {
                unsigned int * neighborhood = getNeighborhood(i, j, nrows, ncols, rmajor);
                unsigned int * controls = getControls(neighborhood);
                unsigned int target = getTarget(neighborhood);
                printNeighborhood(neighborhood, nrows, ncols, rmajor);
                for (unsigned int k = 0; k < nlevels; ++k) {
                    unsigned int level = levels[k+1];
                    multiControlledActivator(qubits, controls, target, level, activator);
                }
                free(neighborhood);
                free(controls);
            }
        }
    }
}

#endif
