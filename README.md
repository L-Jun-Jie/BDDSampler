# BDDSampler
BDD-based bit vector constrained sampler

## 1. Compiling CUDD
```shell
$ tar -xf lib.tar
$ cd lib/cudd-3.0.0
$ ./configure --enable-silent-rules --enable-obj
$ make
```

## 2. Running the program
```shell
$ cd src
$ make
$ ./bddsampler inputfile
```

## 3. Convert dot files to png or pdf files
dot to png:
`dot -Tpng filename.dot -o filename.png`
dot to pdf:
`dot -Tpdf filename.dot -o filename.pdf`

## 4. Next steps
1. Unify the variable bit-widths according to the syntax of system verilog before operation.

2. Configure python test scripts to verify the correctness of logical operations.