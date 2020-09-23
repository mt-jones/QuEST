import os
import itertools
import numpy as np
import functools as ft
import multiprocessing as mp
from scipy.linalg import fractional_matrix_power as fmp


def _apply_tolerance(density):
    density_diagonal = density.diagonal()
    density_diagonal.setflags(write=True)
    valid_probability_mask = density_diagonal >= 0
    invalid_probability_mask = np.logical_not(valid_probability_mask)
    density_diagonal[invalid_probability_mask] = 0
    return density

def _log(matrix):
    nonzero_mask = matrix != 0.j
    invalid_mask = np.logical_and(matrix.real <= 0, matrix.imag == 0)
    valid_mask = np.logical_and(nonzero_mask, np.logical_not(invalid_mask))
    log_matrix = np.zeros(matrix.shape, dtype=complex)
    log_matrix[valid_mask] = np.log2(matrix[valid_mask])
    return log_matrix

def _entropy_operator(density, alpha):
    if alpha == 1:
        return (-np.trace(density @ _log(density)))
    else:
        density_alpha = fmp(density, alpha)
        trace_density_alpha = np.trace(density_alpha)
        log_trace_density_alpha = np.log2(trace_density_alpha)
        return ((1 / (1 - alpha)) * log_trace_density_alpha)

def _calculate_entropy(density, alpha):
    density = _apply_tolerance(density)
    entropy = _entropy_operator(density, alpha)
    return entropy

def entropy(qubit_densities, alpha):
    entropies = [[qubit_density[0], _calculate_entropy(qubit_density[1], alpha)]
                 for qubit_density in qubit_densities]
    return entropies

def _information(densities_args):
    single_qubit_densities, two_qubit_densities, alpha = densities_args
    single_qubit_entropies = entropy(single_qubit_densities, alpha)
    two_qubit_entropies = entropy(two_qubit_densities, alpha)
    mutual_information = []
    for single_qubit_index, single_qubit_entropyi in single_qubit_entropies:
        for single_qubit_jndex, single_qubit_entropyj in single_qubit_entropies:
            if (single_qubit_index != single_qubit_jndex):
                single_qubit_indices = np.array(list(itertools.chain.from_iterable([single_qubit_index, single_qubit_jndex])))
                for two_qubit_index, two_qubit_entropy in two_qubit_entropies:
                    if (single_qubit_indices == two_qubit_index).all():
                        mutual_information.append([two_qubit_index, 0.5 * np.abs(single_qubit_entropyi + single_qubit_entropyj - two_qubit_entropy)])
    return mutual_information

def _get_information_matrix(mutual_information):
    max_mutual_index = 0
    for mutual_index, mutual_info in mutual_information:
        index = max(mutual_index)
        if  max_mutual_index < index:
            max_mutual_index = index
    mutual_information_matrix = np.zeros((max_mutual_index+1, max_mutual_index+1))
    for mutual_index, mutual_info in mutual_information:
        mutual_information_matrix[mutual_index[0], mutual_index[1]] = mutual_info
    lower_symmetric = np.tril(mutual_information_matrix) + np.triu(mutual_information_matrix.transpose(), 1)
    upper_symmetric = np.triu(mutual_information_matrix) + np.tril(mutual_information_matrix.transpose(), -1)
    mutual_information_matrix = 0.5 * (lower_symmetric + upper_symmetric)
    return mutual_information_matrix

def information(single_qubit_densities, two_qubit_densities, alpha):
    nsims = ((len(single_qubit_densities)) if (len(single_qubit_densities) == len(two_qubit_densities))
             else (0))
    with mp.Pool(mp.cpu_count()) as pool:
        mutual_information_list = pool.map(_information, [(single_qubit_densities[i], two_qubit_densities[i], alpha)
                                                          for i in range(nsims)])
        mutual_information_list = pool.map(_get_information_matrix, [mutual_information
                                                                     for mutual_information in mutual_information_list])
    return mutual_information_list

def _clustering(mutual_information_args):
    mutual_information, tolerance = mutual_information_args
    _numerator = fmp(mutual_information, 3)
    numerator = np.trace(_numerator)
    _denominator = fmp(mutual_information, 2)
    np.fill_diagonal(_denominator, 0)
    denominator = np.sum(_denominator)
    return ((numerator / denominator) if (np.abs(denominator) > tolerance)
            else (0))

def clustering(mutual_information_list, tolerance):
    with mp.Pool(mp.cpu_count()) as pool:
        clustering = pool.map(_clustering, [(mutual_information, tolerance)
                                            for mutual_information in mutual_information_list])
    return clustering

def _calculate_polarization(single_qubit_density, index):
    return single_qubit_density.diagonal()[index]

def polarization(single_qubit_densities, index):
    polarization = [[single_qubit_density[0], _calculate_polarization(single_qubit_density[1], index)]
                 for single_qubit_density in single_qubit_densities]
    return polarization
