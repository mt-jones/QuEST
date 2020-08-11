#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "utilities.h"
#include "../../../QuEST/include/QuEST.h"

#pragma once
#if !defined FIVE_SITE_H
#define FIVE_SITE_H
#define NHOOD 5
#define ACTIVATOR hadamard

unsigned int getIndex(unsigned int row,
                      unsigned int col,
                      unsigned int nrows,
                      unsigned int ncols,
                      bool rmajor) {
    if (rmajor) {
        return row * ncols + col;
    } else {
        return col * nrows + row;
    }
}

void setState(char * bitstring,
              unsigned int nrows,
              unsigned int ncols,
              bool rmajor,
              char * state) {
    unsigned int * up = NULL;
    unsigned int * down = NULL;
    unsigned int * right = NULL;
    unsigned int * left = NULL;
    unsigned int * center = NULL;
    if (nrows % 2 == 1 && ncols % 2 == 1) {
        unsigned int row = (unsigned int) floor(nrows / 2);
        unsigned int col = (unsigned int) floor(ncols / 2);
        *center = getIndex(row, col, nrows, ncols, rmajor);
        if (nrows > 1) {
            *up = getIndex(row+1, col, nrows, ncols, rmajor);
            *down = getIndex(row-1, col, nrows, ncols, rmajor);
        }
        if (ncols > 1) {
            *right = getIndex(row, col+1, nrows, ncols, rmajor);
            *left = getIndex(row, col-1, nrows, ncols, rmajor);
        }
    }
    if (!strcmp(state, "W")) {
        if (*up > 0) {
            bitstring[*up] = '1';
        }
        if (*down > 0) {
            bitstring[*down] = '1';
        }
        if (*right > 0) {
            bitstring[*right] = '1';
        }
        if (*left > 0) {
            bitstring[*left] = '1';
        }
    }
}

unsigned int * getNeighborhood(unsigned int row,
                               unsigned int col,
                               unsigned int nrows,
                               unsigned int ncols,
                               bool rmajor) {
    unsigned int * out = NULL;
    if (col >= 0 && col >= 0) {
        if (col-1 >= 0) {
            unsigned int nout = (*out) / sizeof(unsigned int);
            out = (unsigned int *) realloc(out, nout);
            out[nout-1] = getIndex(row, col-1, nrows, ncols, rmajor);  // up
        }
        if (col+1 >= 0) {
            unsigned int nout = (*out) / sizeof(unsigned int);
            out = (unsigned int *) realloc(out, nout);
            out[nout-1] = getIndex(row, col+1, nrows, ncols, rmajor);  // down
        }
        unsigned int nout = (*out) / sizeof(unsigned int);
        out = (unsigned int *) realloc(out, nout);
        out[nout-1] = getIndex(row, col, nrows, ncols, rmajor);  // center
        if (row-1 >= 0) {
            unsigned int nout = (*out) / sizeof(unsigned int);
            out = (unsigned int *) realloc(out, nout);
            out[nout-1] = getIndex(row-1, col, nrows, ncols, rmajor);  // left
        }
        if (row+1 >=0) {
            unsigned int nout = (*out) / sizeof(unsigned int);
            out = (unsigned int *) realloc(out, nout);
            out[nout-1] = getIndex(row+1, col, nrows, ncols, rmajor);  // right
        }
    }
    return out;
}

void updateQubits(Qureg qubits,
                  unsigned int nrows,
                  unsigned int ncols,
                  unsigned int level) {
    // update qubits according to activation level under a specific stride pattern
}

#endif
