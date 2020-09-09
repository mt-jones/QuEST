import os
import itertools
import numpy as np
import functools as ft


def _log(density):
    nonzero_mask = density != 0.j
    invalid_mask = density.real <= 0 and density.imag == 0
    valid_mask = nonzero_mask and not invalid_mask;
    matrix = np.zeros(density.shape, dtype=complex)
    matrix[valid_mask] = np.log2(density[valid_mask])
    return matrix

def _entropy(qubit_densities):
    matrices = [[qubit_density[0], qubit_density[1] @ _log(qubit_density[1])]
                for qubit_density in qubit_densities]
    entropies = [[matrix[0], np.trace(matrix[1])]
                 for matrix in matrices]
    return entropies


def _information(single_qubit_densities, two_qubit_densities):
    single_qubit_entropies = _entropy(single_qubit_densities)
    two_qubit_entropies = _entropy(two_qubit_densities)
    mutual_information = []
    for single_qubit_index, single_qubit_entropyi in single_qubit_entropies:
        for single_qubit_jndex, single_qubit_entropyj in single_qubit_entropies:
            if (single_qubit_index != single_qubit_jndex).any():
                for two_qubit_index, two_qubit_entropy in two_qubit_entropies:
                    single_qubit_indices = np.array(itertools.chain.from_iterable([single_qubit_index, single_qubit_jndex]))
                    if (single_qubit_indices == two_qubit_index).all():
                        mutual_information.append([two_qubit_index, 0.5 * np.abs(single_qubit_entropyi + single_qubit_entropyj - two_qubit_entropy)])
    return mutual_information

def _process_information(mutual_information):
    max_mutual_index = 0
    for mutual_index, mutual_info in mutual_information:
        index = max(mutual_index)
        if index < max_mutual_index:
            max_mutual_index = index
    mutual_information_matrix = np.zeros(max_mutual_index+1)
    for mutual_index, mutual_info in mutual_information:
        mutual_information_matrix[mutual_index[0], mutual_index[0]] = mutual_info
    return mutual_information_matrix

def information(single_qubit_densities_list, two_qubit_densities_list):
    if len(single_qubit_densities_list) == len(two_qubit_densities_list):
        mutual_information_list = [_information(single_qubit_densities_list[index], two_qubit_densities_list[index])
                                   for index, _ in enumerate(single_qubit_densities_list)]
        mutual_information_list = [_process_information(mutual_information)
                                   for mutual_information in mutual_information_list]
        return mutual_information_list