# data schema

## `{0}traj_{1}qubits_{2}r_{3}c_{4}ru_{5}ic`

0. the trajectory index
1. the total number of qubits
2. the number of rows in the qubit array
3. the number of columns in the qubit array
4. the rule designation
5. the initial condition designation

## rules

A rule is defined by its neighborhood. Given a central qubit, the list of qubits that control the evolution of the central qubit is defined to be the central qubit's neighborhood.

* 1-d; central qubit index `i`, neighborhood: `[i-(n//2), i-(n//2-1), i-(n//2-2), ..., i-(n//2-(n//2-1)), i-(n//2-(n//2+1)), i+(2), i+(3), i+(4), ..., i+(n//2)]`, neighborhood size: `n+1` (to account for the central qubit)
* 2-d; central qubit index `(i, j)`, neighborhood: `[(i-(n//2), j), (i-(n//2-1), j), (i-(n//2-2), j), ..., (i+(1), j), ..., (i+(n//2), j), (i, j-(n//2)), (i, j-(n//2-1)), (i, j-(n//2-2)), ..., (i, j+(1)), (i, j+(2)), ... (i, j+(n//2))]`, neighborhood size: `2n+1` (to ignore diagonal indices, and to account for the central qubit)

For our purposes, we will only consider neighborhoods whose size is odd. With this in mind, consider a 1-d neighborhood whose size is `size=3 -> n=2`. The number of z-spins in the neighborhood is the neighborhood's level. For a 3-site neighborhood in 1-d, there are `2**3` (totalistic) rules corresponding to the three valid neighborhood levels (`level = [0, 1, 2]` as the central qubit is not a control). The rules are as follows:

0. control on `level = []`; binary: `000`
1. control on `level = [0]`; binary: `001`
2. control on `level = [1]`; binary: `010`
3. control on `level = [0, 1]`; binary: `011`
4. control on `level = [2]`; binary: `100`
5. control on `level = [0, 2]`; binary: `101`
6. control on `level = [1, 2]`; binary: `110`
7. control on `level = [0, 1, 2]`; binary: `111`

## initial conditions

In order for a rule to induce nontrivial evolution in a circuit cycle, there must be a seed state. The seed state or initial condition can produce dynamics with complexity under the application of the appropriate rule. Like the rule, the initial condition is defined by the neighborhood of the system's global center (excluding the center). For example, in 1-d with a three-site neighborhood:

0. `[000 ... 000 ... 000]`; binary: `00`
1. `[000 ... 001 ... 000]`; binary: `01`
2. `[000 ... 100 ... 000]`; binary: `10`
3. `[000 ... 101 ... 000]`; binary: `11`

Similarly, in 1-d with a five-site neighborhood:

0. `[000 ... 00000 ... 000]`; binary: `0000`
1. `[000 ... 00001 ... 000]`; binary: `0001`
2. `[000 ... 00010 ... 000]`; binary: `0010`
3. `[000 ... 00011 ... 000]`; binary: `0011`
4. ... and so on

In 2-d with a five-site neighborhood, the initial condition bits are ordered: `[up, down, left, right, center]` where `center` is the floored center of the qubit array, and `[up, ...]` means "one qubit `up` from center" etc..
