#include "utilities.h"
#include "../../logging/logging.h"

#pragma once
#if !defined FIVE_SITE_LOGGING_H
#define FIVE_SITE_LOGGING_H

void printProductState(char * state,
                       unsigned int nrows,
                       unsigned int ncols,
                       bool rmajor) {
    unsigned int nqubits = nrows * ncols;
    reverse(state, nqubits);
    unsigned int bsize = 80;
    const char * format = "%2c%1s|";
    char buffer[bsize];
    int length = sprintf(buffer, "|");
    for (unsigned int i = 0; i < nrows; ++i) {
        length = length + sprintf(buffer, format, '0', "");
    }
    char pad = '-';
    char padding[length+1];
    for (int i = 0; i < length; ++i) {
        padding[i] = pad;
    }
    padding[length] = '\0';
    printf("%s\n", padding);
    for (unsigned int j = 0; j < ncols; ++j) {
        sprintf(buffer, "|");
        printf("%s", buffer);
        for (unsigned int i = 0; i < nrows; ++i) {
            unsigned int index = get1dIndex(i, j, nrows, ncols, rmajor);
            sprintf(buffer, format, state[index], "");
            printf("%s", buffer);
        }
        printf("\n%s\n", padding);
    }
    reverse(state, nqubits);
}

void printProbabilityGrid(Qureg qubits,
                          unsigned int nrows,
                          unsigned int ncols,
                          int outcome,
                          bool rmajor) {
    unsigned int bsize = 80;
    const char * format = "%1s%5.2f%1s|";
    char buffer[bsize];
    int length = sprintf(buffer, "|");
    for (unsigned int i = 0; i < nrows; ++i) {
        length = length + sprintf(buffer, format, "", (double) 0, "");
    }
    char pad = '-';
    char padding[length+1];
    for (int i = 0; i < length; ++i) {
        padding[i] = pad;
    }
    padding[length] = '\0';
    printf("%s\n", padding);
    for (unsigned int j = 0; j < ncols; ++j) {
        sprintf(buffer, "|");
        printf("%s", buffer);
        for (unsigned int i = 0; i < nrows; ++i) {
            unsigned int index = get1dIndex(i, j, nrows, ncols, rmajor);
            double prob = calcProbOfOutcome(qubits, index, outcome);
            sprintf(buffer, format, "", prob, "");
            printf("%s", buffer);
        }
        printf("\n%s\n", padding);
    }
}

void printNeighborhood(unsigned int * neighborhood,
                       unsigned int nrows,
                       unsigned int ncols,
                       bool rmajor) {
    unsigned int cindex = (unsigned int) floor(neighborhood[0] / 2);
    unsigned int index = neighborhood[cindex+1];
    unsigned int * center = get2dIndex(index, nrows, ncols, rmajor);
    unsigned int bsize = 80;
    char buffer[bsize];
    int length = sprintf(buffer, "| %2s [%2u; %2u, %2u]%1s", "c:", index, center[1], center[2], "");
    for (unsigned int i = 0; i < neighborhood[0]; ++i) {
        if (i+1 != cindex+1) {
            unsigned int val = neighborhood[i+1];
            if(val != INVALID) {
                unsigned int * control = get2dIndex(val, nrows, ncols, rmajor);
                length = length + sprintf(buffer, "%2s [%2u; %2u, %2u]%1s", "ctrl:", val, control[1], control[2], "");
                free(control);
            }
        }
    }
    length = length + sprintf(buffer, "|");
    char pad = '-';
    char padding[length+1];
    for (int i = 0; i < length; ++i) {
        padding[i] = pad;
    }
    padding[length] = '\0';
    printf("%s\n", padding);
    sprintf(buffer, "| %2s [%2u; %2u, %2u]%1s", "c:", index, center[1], center[2], "");
    printf("%s", buffer);
    for (unsigned int i = 0; i < neighborhood[0]; ++i) {
        if (i+1 != cindex+1) {
            unsigned int val = neighborhood[i+1];
            if(val != INVALID) {
                unsigned int * control = get2dIndex(val, nrows, ncols, rmajor);
                sprintf(buffer, "%2s [%2u; %2u, %2u]%1s", "ctrl:", val, control[1], control[2], "");
                printf("%s", buffer);
                free(control);
            }
        }
    }
    sprintf(buffer, "|");
    printf("%s", buffer);
    printf("\n%s\n", padding);
    free(center);
}

#endif
