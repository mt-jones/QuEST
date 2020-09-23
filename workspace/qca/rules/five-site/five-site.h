#include "logging.h"
#include "nhood.h"
#include "utilities.h"
#include "../../external/external.h"
#include <sys/types.h>

#pragma once
#if !defined FIVE_SITE_H
#define FIVE_SITE_H

const char * argNames[] = {"rmajor",
                           "verbose",
                           "nrows",
                           "ncols",
                           "rule",
                           "activator",
                           "ic",
                           "gate-mode",
                           "open-boundaries",
                           "trajectory",
                           "gate-err",
                           "ncycles"};

const unsigned int numArgNames = sizeof(argNames) / sizeof(char *);

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
    if (row == 0) {
        left = INVALID;
    }
    if (row == nrows-1) {
        right = INVALID;
    }
    if (col == 0) {
        up = INVALID;
    }
    if (col == ncols-1) {
        down = INVALID;
    }
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
                  bool rmajor,
                  bool verbose,
                  enum qubitGateMode mode,
                  bool openBoundaries,
                  qreal qubitGateErr) {
    unsigned int frow, lrow;
    unsigned int fcol, lcol;
    if (openBoundaries) {
        frow = 0;
        lrow = nrows;
        fcol = 0;
        lcol = ncols;
    } else {
        frow = 1;
        lrow = nrows - 1;
        fcol = 1;
        lcol = ncols - 1;
    }
    unsigned int nlevels = levels[0];
    for (unsigned int offset = 0; offset <= 1; ++offset) {
        for (unsigned int i = frow; i < lcol; ++i) {
            for (unsigned int j = fcol+(i+offset)%2; j < lcol; j = j+2) {
                unsigned int * neighborhood = getNeighborhood(i, j, nrows, ncols, rmajor);
                unsigned int * controls = getControls(neighborhood);
                unsigned int target = getTarget(neighborhood);
                if (verbose) {
                    printNeighborhood(neighborhood, nrows, ncols, rmajor);
                }
                for (unsigned int k = 0; k < nlevels; ++k) {
                    unsigned int level = levels[k+1];
                    multiControlledActivator(qubits, controls, target, level, activator, mode, qubitGateErr);
                }
                free(neighborhood);
                free(controls);
            }
        }
    }
}

#endif
