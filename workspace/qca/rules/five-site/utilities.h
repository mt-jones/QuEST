#include "../../utilities/utilities.h"

#pragma once
#if !defined FIVE_SITE_UTILITIES_H
#define FIVE_SITE_UTILITIES_H

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

#endif
