# Compiling and Executing OpenMP Programs

After compiling with  `gcc −O0 −fopenmp −o ex1 ex1.c`, and executing, we get the following output:

```bash
$ ./ex1 8
I am the master thread 0 and I start
Starting Region 1 
Region 1  thread 0 of team 8 (max_num_threads is 8)
Region 1  thread 2 of team 8 (max_num_threads is 8)
Region 1  thread 5 of team 8 (max_num_threads is 8)
Region 1  thread 3 of team 8 (max_num_threads is 8)
Region 1  thread 7 of team 8 (max_num_threads is 8)
Region 1  thread 1 of team 8 (max_num_threads is 8)
Region 1  thread 4 of team 8 (max_num_threads is 8)
Region 1  thread 6 of team 8 (max_num_threads is 8)
End of Region 1
Starting Region 2 
Region 2  thread 0 of team 4 (max_num_threads is 8)
Region 2  thread 2 of team 4 (max_num_threads is 8)
Region 2  thread 3 of team 4 (max_num_threads is 8)
Region 2  thread 1 of team 4 (max_num_threads is 8)
End of Region 2
Starting Region 3 
Region 3  thread 0 of team 2 (max_num_threads is 8)
Region 3  thread 1 of team 2 (max_num_threads is 8)
End of Region 3
I am the master thread 0 and I complete

```

1. Since our machine has 8 cores, all 8 cores execute parallelly. 2nd region half the cores (4) execute parallelly. In 3rd region, a quarter (2) cores execute parallelly.

2. We see that during different runs, the order of execution (display of print statements) varies at each run, and is assumed to be random in nature.

---

# Performance Analysis of Vector and Matrix Operations

## 2. Measuring GFLOPS
To get the number of giga floating point operations per second (GFLOPS or GFLOP/s), we can make use of the difference (`av-residu`) which gives us the number of cycles, along with processor frequency (`freq` or `PROC_FREQ`) as follows:

`GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu)`

To get `total FP ops` in the case of `add_vector()`:

* We manipulate vectors of size `datasize` (or `DEF_SIZE`) in the code.
* The only FP op in each iteration is `X[i] + Y[i]`
* So the total FP ops = `DEF_SIZE * 1`

## 3. Speedup Analysis with different number of threads

We do the GFLOP/s counting for addition operation (`add_vectors1()` function)

```
maxnthreads:    2

parallel:        0.149 GFLOP/s
sequential:      0.234 GFLOP/s
```

```
maxnthreads:    4

parallel:        0.133 GFLOP/s
sequential:      0.717 GFLOP/s
```

```
maxnthreads:    8

parallel:       0.003 GFLOP/s
sequential:     0.337 GFLOP/s
```

We see that sequential part has more FLOP per second, implying that sequential part is faster. This could be due to having smaller data size, wherein the cost of parallelizing the task itself has more overheads that simply executing it sequentially.

Parallelization has benefits when data size is large.


```
N=1000000
maxnthreads:    8

parallel:       0.328 GFLOP/s
                4874338 cycles

sequential:     0.413 GFLOP/s
                3876899 cycles
```

## 4. `-O2` compile option

We now compile with `-O2` option:
`gcc -O2 -fopenmp -o ex2 ex2.c`

Previously:
```
Testing with Vectors of size 100 -- Matrices of size 100 X 100                                                                                                                                                                                                                Max number of threads: 8                                                                                                               =============== ADD ==========================================                                                                         OpenMP static loop:              3011 cycles                                                                                           GFLOPS = 0.053 GFLOP per second                                                                                                                                                                                                                                               OpenMP dynamic loop:             6260 cycles                                                                                           GFLOPS = 0.026 GFLOP per second                                                                                                        ==============================================================                                                                                                                                                                                                                ==================== DOT =====================================                                                                         dot OpenMP static loop:                  2036 cycles                                                                                   GFLOPS = 0.157 GFLOP per second                                                                                                                                                                                                                                               dot OpenMP dynamic loop:                 6424 cycles                                                                                   GFLOPS = 0.050 GFLOP per second                                                                                                                                                                                                                                               dot OpenMP static unrolled loop:         2070 cycles                                                                                   GFLOPS = 0.155 GFLOP per second                                                                                                        =============================================================
```

With `-O2` option:

```
Testing with Vectors of size 100 -- Matrices of size 100 X 100                                                                                                                                                                                                                Max number of threads: 8                                                                                                               =============== ADD ==========================================                                                                         OpenMP static loop:              1783 cycles                                                                                           GFLOPS = 0.090 GFLOP per second                                                                                                                                                                                                                                               OpenMP dynamic loop:             7697 cycles                                                                                           GFLOPS = 0.021 GFLOP per second                                                                                                        ==============================================================                                                                                                                                                                                                                ==================== DOT =====================================                                                                         dot OpenMP static loop:                  2146 cycles                                                                                   GFLOPS = 0.149 GFLOP per second                                                                                                                                                                                                                                               dot OpenMP dynamic loop:                 7678 cycles                                                                                   GFLOPS = 0.042 GFLOP per second                                                                                                                                                                                                                                               dot OpenMP static unrolled loop:         2099 cycles                                                                                   GFLOPS = 0.152 GFLOP per second                                                                                                        =============================================================
```

Result: inconclusive

__Operations on Matrices__

* _Matrix-Vector_

As usual, we have our formulae for calculating FLOPS:

`GFLOPS  = ( (total FP ops) * (freq) ) / (av-residu)`

To get `total FP ops` in the case of `mult_mat_vec{0-2}`, we have (N * N) multiplications, and (N-1 * N) additions to sum up the (N * N) multiplications, and so:


total FP ops = `(DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE)`

Experiments: Experiments were performed with sequential, OpenMP static loop, as well as OpenMP static unrolled loop. The result was still unconclusive, with static unrolled's GFLOPS sometimes being more than static loop's and sometimes less on different runs.

* _Matrix-Matrix_

For Matrix-Matrix the total FP ops are:

total FP ops = `(DEF_SIZE * DEF_SIZE * DEF_SIZE) + ((DEF_SIZE - 1) * DEF_SIZE * DEF_SIZE)`

since matrix is basically an extension of vector, so there is an increase in dimension for both multiplication as well as addition operations.

With `DEF_SIZE = 1024`:

```
======================== Mult Mat Mat =====================================
Sequential Matrix Matrix Multiplication:         15345386720 cycles
GFLOPS = 0.224 GFLOP per second
OpenMP static loop matrix matrix multiplication: 6862852407 cycles
GFLOPS = 0.500 GFLOP per second
OpenMP static unrolled loop matrix matrix multiplication: 3853488397 cycles
GFLOPS = 0.891 GFLOP per second
===================================================================

```
Observations: We see that OpenMP has an advantage only when we use matrices of very large dimensions (1024 X 1024 in this case).

# OpenMP Loop Scheduling

