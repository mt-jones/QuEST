import os
import struct
import numpy as np
import functools as ft


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

def _read_binary_qubit_density_data(filepaths):
    binary_qubit_density_data = []
    if not isinstance(filepaths, list):
        filepaths = [filepaths]
    for filepath in filepaths:
        if os.path.exists(filepath):
            with open(filepath, "rb") as file:
                binary_qubit_density_data.append(file.read())
    return binary_qubit_density_data

def _process_binary_qubit_density_data(binary_qubit_density_data):
    if not isinstance(binary_qubit_density_data, list):
        binary_qubit_density_data = [binary_qubit_density_data]
    lengths = []
    for binary_qubit_density_datum in binary_qubit_density_data:
        lengths.append(struct.unpack(types["header"], binary_qubit_density_datum[0:size["header"]]))
    qlengths = []
    plengths = []
    clengths = []
    for length in lengths:
        qlengths.append(length[0])
        plengths.append(length[1])
        clengths.append(length[2])
    qindices = []
    index = size["header"]
    for idx, qlength in enumerate(qlengths):
        binary_qubit_density_datum = binary_qubit_density_data[idx]
        _qindices = []
        for jdx in range(qlength):
            nvals = struct.unpack(types["int"], binary_qubit_density_datum[index:index+size["int"]])[0]
            index = index + size["int"]
            vals = struct.unpack(types["int"]*nvals, binary_qubit_density_datum[index:index+size["int"]*nvals])
            _qindices.append(vals)
            index = index + size["int"] * nvals
        if len(_qindices) > 0:
            qindices.append(_qindices)
    pindices = []
    for idx, plength in enumerate(plengths):
        binary_qubit_density_datum = binary_qubit_density_data[idx]
        _pindices = []
        for jdx in range(plength):
            nvals = struct.unpack(types["int"], binary_qubit_density_datum[index:index+size["int"]])[0]
            index = index + size["int"]
            vals = struct.unpack(types["int"]*nvals, binary_qubit_density_datum[index:index+size["int"]*nvals])
            _pindices.append(vals)
            index = index + size["int"] * nvals
        if len(_pindices) > 0:
            pindices.append(_pindices)
    coeffs = []
    for idx, clength in enumerate(clengths):
        binary_qubit_density_datum = binary_qubit_density_data[idx]
        _coeffs = []
        for jdx in range(clength):
            nvals = int(struct.unpack(types["real"], binary_qubit_density_datum[index:index+size["real"]])[0])
            index = index + size["real"]
            vals = struct.unpack(types["real"]*nvals, binary_qubit_density_datum[index:index+size["real"]*nvals])[0]
            _coeffs.append(vals)
            index = index + size["real"] * nvals
        if len(_coeffs) > 0:
            coeffs.append(_coeffs)
    qubit_density_data = []
    for idx, _ in enumerate(binary_qubit_density_data):
        _qindices = qindices[idx]
        _pindices = pindices[idx]
        _coeffs = coeffs[idx]
        _qubit_density_data = [(_qindices[jdx], _pindices[jdx], _coeffs[jdx])
                               for jdx in range(len(_qindices))]
        qubit_density_data.append(_qubit_density_data)
    return qubit_density_data

def _get_qubit_density_terms(qubit_density_data):
    qubit_density_terms = []
    for qubit_density_datum in qubit_density_data:
        _qubit_density_terms = []
        for local_qubit_density in qubit_density_datum:
            qindices = local_qubit_density[0]
            pindices = local_qubit_density[1]
            coeff = local_qubit_density[2]
            matrices = [paulis[pindex] for pindex in pindices]
            local_density_matrix = ft.reduce(lambda a, b: np.kron(a, b), matrices)
            _qubit_density_terms.append((qindices, local_density_matrix, coeff))
        if len(_qubit_density_terms) > 0:
            qubit_density_terms.append(_qubit_density_terms)
    return qubit_density_terms

def _get_qubit_densities(qubit_density_terms):
    qubit_densities = []
    for local_qubit_density in qubit_density_terms:
        unique_qindices = np.unique([local_qubit_density[index][0] for index, _ in enumerate(local_qubit_density)], axis=0)
        _qubit_densities = [[unique_qindex, np.zeros([2**len(unique_qindex), 2**len(unique_qindex)], dtype=complex)]
                            for unique_qindex in unique_qindices]
        for index, unique_qindex in enumerate(unique_qindices):
            for qubit_density_term in local_qubit_density:
                if (unique_qindex == np.array(qubit_density_term[0])).all():
                    density_matrix = qubit_density_term[1]
                    coeff = qubit_density_term[2]
                    _qubit_densities[index][1] = _qubit_densities[index][1] + coeff * density_matrix;
        qubit_densities.append(_qubit_densities)
    return qubit_densities

def get_qubit_densities(filepaths):
    binary_qubit_density_data = _read_binary_qubit_density_data(filepaths)
    qubit_density_data = _process_binary_qubit_density_data(binary_qubit_density_data)
    qubit_density_terms = _get_qubit_density_terms(qubit_density_data)
    qubit_densities = _get_qubit_densities(qubit_density_terms)
    return qubit_densities