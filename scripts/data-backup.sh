#!/bin/bash
tar cjvf seq1.tar.bz seq1/
tar cvf seq3.tar --exclude '*.png' seq3/
tar -rf seq3.tar seq3/img/*.png
bzip2 seq3.tar
tar cvf seq9.tar --exclude '*.png' --exclude '*.tif' seq9/
bzip2 seq9.tar
tar cvf seq12.tar --exclude '*.png' --exclude '*.tif' seq12/
bzip2 seq12.tar
