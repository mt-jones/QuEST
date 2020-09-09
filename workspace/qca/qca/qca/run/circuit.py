import os
import struct
import numpy as np
import functools as ft
import subprocess


def cycle(ncycles="1", nrows="3", ncols="3", rule="12", activator="Sqrt-Hadamard", ic="W", rmajor="true", verbose="false", qca_root=os.path.abspath(os.getcwd())):
    exe_qca = os.path.join(qca_root, os.path.abspath("./build/exe_qca"))
    if os.path.isfile(exe_qca):
        cmd = f"{exe_qca} rmajor={rmajor} verbose={verbose} nrows={nrows} ncols={ncols} rule={rule} activator={activator} ic={ic} ncycles={ncycles}"
        process = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = process.communicate()
        return (cmd, out.decode(), err.decode())
    else:
        print("qca exe does not exist")
