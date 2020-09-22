import os
import struct
import numpy as np
import itertools as it
import functools as ft
import multiprocessing as mp


paulis = [
    np.array([[1,   0],
              [0,   1]]),
    np.array([[0,   1],
              [1,   0]]),
    np.array([[0,  -1.j],
              [1.j, 0]]),
    np.array([[1,   0],
              [0,  -1]])
]

types = {
    "header": "iii",
    "int": "i",
    "real": "d"
}

size = {
    "header": struct.calcsize("iii"),
    "int": struct.calcsize("i"),
    "real": struct.calcsize("d")
}

def _read_binary_qubit_density_data(filepath):
    filepath = os.path.abspath(filepath)
    binary_qubit_density_data = None
    if os.path.exists(filepath):
        with open(filepath, "rb") as file:
            binary_qubit_density_data = file.read()
    return binary_qubit_density_data

def _process_binary_qubit_density_data(binary_qubit_density_data):
    length = struct.unpack(types["header"], binary_qubit_density_data[0:size["header"]])
    qlength = length[0]
    plength = length[1]
    clength = length[2]
    qindices = []
    index = size["header"]
    for idx in range(qlength):
        nvals = struct.unpack(types["int"], binary_qubit_density_data[index:index+size["int"]])[0]
        index = index + size["int"]
        vals = struct.unpack(types["int"]*nvals, binary_qubit_density_data[index:index+size["int"]*nvals])
        qindices.append(vals)
        index = index + size["int"] * nvals
    pindices = []
    for jdx in range(plength):
        nvals = struct.unpack(types["int"], binary_qubit_density_data[index:index+size["int"]])[0]
        index = index + size["int"]
        vals = struct.unpack(types["int"]*nvals, binary_qubit_density_data[index:index+size["int"]*nvals])
        pindices.append(vals)
        index = index + size["int"] * nvals
    coeffs = []
    for jdx in range(clength):
        nvals = int(struct.unpack(types["real"], binary_qubit_density_data[index:index+size["real"]])[0])
        index = index + size["real"]
        vals = struct.unpack(types["real"]*nvals, binary_qubit_density_data[index:index+size["real"]*nvals])[0]
        coeffs.append(vals)
        index = index + size["real"] * nvals
    qubit_density_data = (
        qindices,
        pindices,
        coeffs
    )
    return qubit_density_data

def _get_qubit_density_terms(qubit_density_data):
    qubit_density_terms = []
    qindices = qubit_density_data[0]
    pindices = qubit_density_data[1]
    coeff = qubit_density_data[2]
    qubit_density_terms = []
    for idx, _ in enumerate(qindices):
        matrices = [paulis[pindex] for pindex in list(it.chain.from_iterable([list(pindices[idx])]))]
        local_density_matrix = ft.reduce(lambda a, b: np.kron(a, b), matrices)
        qubit_density_terms.append((qindices[idx], local_density_matrix, coeff[idx]))
    return qubit_density_terms

def _normalize(density):
    return density / np.trace(density)

def _get_qubit_densities(qubit_density_terms):
    unique_qindices = np.unique([qubit_density_term[0]
                                 for qubit_density_term in qubit_density_terms], axis=0)
    _qubit_density_terms = [[qubit_density_term
                             for qubit_density_term in qubit_density_terms if (qubit_density_term[0] == unique_qindex).all()]
                            for unique_qindex in unique_qindices]
    qubit_densities = [[qubit_density_term[0][0], _normalize(np.sum([np.multiply(term[1], term[2])
                                                                     for term in qubit_density_term], axis=0))]
                       for qubit_density_term in _qubit_density_terms]
    return qubit_densities

def qubit_densities(filepaths):
    if not isinstance(filepaths, list):
        filepaths = [filepaths]
    with mp.Pool(mp.cpu_count()) as pool:
        binary_qubit_density_data = pool.map(_read_binary_qubit_density_data, filepaths)
        qubit_density_data = pool.map(_process_binary_qubit_density_data, binary_qubit_density_data)
        qubit_density_terms = pool.map(_get_qubit_density_terms, qubit_density_data)
        qubit_densities = pool.map(_get_qubit_densities, qubit_density_terms)
    return qubit_densities
