#!/bin/csh
#make clean comp_gold
#make clean
make cleanall

make comp_gold
./rast_gold out.ppm $EE271_VECT/vec_271_01_sv.dat

