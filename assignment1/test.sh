#!/bin/csh
#make clean comp_gold
#make clean
make cleanall

make comp_gold

# Compare the image with the reference image
#compare -compose src out.ppm ../vect/vec_271_01_sv_ref.ppm diff.ppm

./rast_gold out.ppm ../vect/vec_271_00_sv.dat
diff out.ppm ../vect/vec_271_00_sv_ref.ppm

./rast_gold out.ppm ../vect/vec_271_01_sv.dat
diff out.ppm ../vect/vec_271_01_sv_ref.ppm

./rast_gold out.ppm ../vect/vec_271_02_sv.dat
diff out.ppm ../vect/vec_271_02_sv_ref.ppm

./rast_gold out.ppm ../vect/vec_271_03_sv_short.dat
diff out.ppm ../vect/vec_271_03_sv_short_ref.ppm

./rast_gold out.ppm ../vect/vec_271_04_sv.dat
diff out.ppm ../vect/vec_271_04_sv_ref.ppm


