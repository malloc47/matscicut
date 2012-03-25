#!/bin/bash
matlab -nodisplay -nojvm -r "addpath('/home/malloc47/src/projects/matsci/matscicut/matlab'); evaluate_file('$1','$2','$3',1:10); exit;"
stty echo
