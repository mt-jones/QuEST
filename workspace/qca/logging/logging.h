#include "../external/external.h"

#pragma once
#if !defined LOGGING_H
#define LOGGING_H

void printStatusMessage(char * message) {
        char buffer[80];
        int length = sprintf(buffer, "| %s |", message);
        char pad = '-';
        char padding[length+1];
        for (int i = 0; i < length; ++i) {
            padding[i] = pad;
        }
        padding[length] = '\0';
        printf("%s\n", padding);
        printf("%s\n", buffer);
        printf("%s\n", padding);
}

#endif
