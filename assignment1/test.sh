#!/bin/csh
#make clean comp_gold
#make clean
make cleanall

make comp_gold

# Compare the image with the reference image
#compare -compose src out.ppm ../vect/vec_271_01_sv_ref.ppm diff.ppm

./rast_gold vec_271_00_sv.ppm ../vect/vec_271_00_sv.dat
./rast_gold vec_271_01_sv.ppm ../vect/vec_271_01_sv.dat
./rast_gold vec_271_02_sv.ppm ../vect/vec_271_02_sv.dat
./rast_gold vec_271_03_sv_short.ppm ../vect/vec_271_03_sv_short.dat
./rast_gold vec_271_04_sv.ppm ../vect/vec_271_04_sv.dat

diff vec_271_00_sv.ppm ../vect/vec_271_00_sv_ref.ppm
diff vec_271_01_sv.ppm ../vect/vec_271_01_sv_ref.ppm
diff vec_271_02_sv.ppm ../vect/vec_271_02_sv_ref.ppm
diff vec_271_03_sv_short.ppm ../vect/vec_271_03_sv_short_ref.ppm
diff vec_271_04_sv.ppm ../vect/vec_271_04_sv_ref.ppm

