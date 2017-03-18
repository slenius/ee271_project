set OUT_FILE = "datanalysis.result"
set DAT_DIR = "/usr/class/ee271/project/vect"

make comp_gold
rm -rf $OUT_FILE
foreach file ($DAT_DIR/*.dat)
  echo processing $file
  ./rast_gold out.ppm $file | grep '^###ANALYSIS' >> datAnalysis.output
end

