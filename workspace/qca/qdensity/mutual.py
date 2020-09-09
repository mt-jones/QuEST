import os
import numpy as np
import functools as ft


def information(single_qubit_densities, two_qubit_densities):
    if len(single_qubit_densities) == len(two_qubit_densities):
        return 0