Simu simple LBM (Lattice Boltzmann Method)
==========================================

Introduction
------------

This project implement the Lattice Boltzmann Method for fluid simulation.
It is used for teaching parallel MPI programming at université and was
started in 2011 for a parallel optimization course at Versailles.

Compiling
---------

To compile you need an MPI implementation (openmpi or mpich or ...) and a C
compiler (gcc).

If you want to change the compile options you can edit the first lines of the 
Makefile file.

To compile you just have to run :

```sh
make
```

Running
-------

You can run the `lbm` executable with the `mpirun` command and select the exercise
by using `-e {EXERCICE_ID}` option. By default it use the exercise 0 which only
allow sequential run with a uniq process.

```sh
mpirun -np 8 ./lbm --exercise 1
# Or
mpirun -np 8 ./lbm -e 1
```

For benchmarking you can disable the output:

```sh
mpirun -np 8 ./lbm --exercise 1 --no-out
# Or
mpirun -np 8 ./lbm -e 1 -n
```

You can edit the config file `config.txt` and can use nother file passed by
parameter:

```sh
mpirun -np 8 ./lbm -e 1 config-other.txt
```

Test communication pattern
--------------------------

When developping arround the communication pattern you can more easily debug
by using the `check_comm` executable which just reproduce the communication
and display the state in the terminal. The gray numbers are the ghost cells.

```sh
mpirun -np 8 ./check_comm --exercise 1
# Or
mpirun -np 8 ./check_comm -e 1
```

Final rendering
---------------

After computing you can make the rendering to get an animated GIF by using the
`gen_animate_gif.sh` script based on `gnuplot`.

```sh
./gen_animate_gif.sh resultat.raw output.gif
```

Benchmarking
------------

You can enable the weak scaling option if you want to automatically grow the
mesh for performance measurement by using the -s option which will grow the mesh
size by the given factor (as close as possible to keep multiple for width
& height).

```sh
mpirun -np 8 ./lbm --exercise 6 --scale 8 --no-out
# Or
mpirun -np 8 ./lbm -e 6 -s 8 --n
```

Generate source archive
-----------------------

You can generate a source archive using:

```sh
make archive
```

License
-------

This code is distributed under BSD license.
