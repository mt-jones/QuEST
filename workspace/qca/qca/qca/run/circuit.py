import os
import struct
import numpy as np
import functools as ft
import subprocess


def cycle(ncycles="1",
          nrows="3",
          ncols="3",
          rule="12",
          activator="Sqrt-Hadamard",
          ic="0",
          gate_mode="5",
          open_boundaries="false",
          trajectory="0",
          gate_err="0",
          rmajor="true",
          verbose="false",
          execute=False,
          qca_root="./"):
    exe_qca = os.path.join(qca_root, "build/exe_qca")
    if os.path.isfile(exe_qca):
        cmd = (f"{exe_qca} " +
               f"rmajor={rmajor} " +
               f"verbose={verbose} " +
               f"nrows={nrows} " +
               f"ncols={ncols} " +
               f"rule={rule} " +
               f"activator={activator} " +
               f"ic={ic} " +
               f"gate-mode={gate_mode} " +
               f"open-boundaries={open_boundaries} " +
               f"trajectory={trajectory} " +
               f"gate-err={gate_err} " +
               f"ncycles={ncycles}")
        out, err = "", ""
        if execute:
            process = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = process.communicate()
            out, err = out.decode(), err.decode()
        ncycles = int(ncycles)
        trajectory = int(trajectory)
        nrows = int(nrows)
        ncols = int(ncols)
        rule = int(rule)
        gate_mode = int(gate_mode)
        gate_err = float(gate_err)
        outdir = os.path.abspath(f"data/{trajectory}traj_{nrows*ncols}qubits_{nrows}r_{ncols}c_{rule}ru_{ic}ic_{gate_mode}mode_{gate_err:4.2f}err")
        outfiles = [[os.path.join(outdir, f"1_qubit_density_{cycle}_cycle.bin"), os.path.join(outdir, f"2_qubit_density_{cycle}_cycle.bin")]
                    for cycle in range(ncycles)]
        return (cmd, out, err, outfiles)
    else:
        print("qca exe does not exist")
