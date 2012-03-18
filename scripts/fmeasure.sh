#!/bin/bash
/usr/local/MATLAB/R2010b/bin/matlab -nodisplay -nojvm -r "addpath('/home/malloc47/src/projects/matsci/matscicut/matlab'); evaluate_file('$1','$2','$3'); exit;"
stty echo
