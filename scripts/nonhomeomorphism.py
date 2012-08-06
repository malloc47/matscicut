#!/usr/bin/env python
import os,sys
import numpy as np
import scipy
from scipy import ndimage

def main(*args):
    if(len(args) < 1):
        return 1

    l = []

    for f in args[1:]:
        labels = np.genfromtxt(f,dtype='int16')
        l.append(labels.max())

    total = np.mean(np.array(l))
    avgdiff = np.mean(np.absolute(np.gradient(np.array(l))))

    print("Total: " + str(total))
    print("Avg Differences: " + str(avgdiff))
    print("AvgDiff/Total: " + str(avgdiff/total))

    return 0

if __name__ == '__main__':
    sys.exit(main(*sys.argv))
