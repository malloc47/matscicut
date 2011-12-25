# matscicut
propagated segmentation for materials images

## Background

This is the raw, uncensored research code for the forthcoming paper,
"Combining global labeling and local relabeling for metallic image
segmentation" in the SPIE Computational Imaging X conference. While
written in C++, it is not idiomatic as it is translated from a
previous MATLAB implementation for performance reasons.

## Dependencies

This project requires 

* OpenCV

  Originally developed for <2.1, but now only runs on >2.1 unless you
  revert commit 08817c06835...
  
* Andrew Delong and Olga Veksler's
[Multi-label optimization library](http://vision.csd.uwo.ca/code/)
([GCO](http://vision.csd.uwo.ca/code/gco-v3.0.zip)) (GCO)

* [cvblobslib](http://opencv.willowgarage.com/wiki/cvBlobsLib) 

  Developed with v6 in mind, which can be patched to work with OpenCV
  > 2.1.

## Usage

The research nature of this code did not mandate a large amount of
flexibility. You will have to manually configure
* the makefile, to point to gco and cvblobslib, both compiled into
  statically-linked libraries
* various configuration options in `include/matscicut.h`, which
  control various parameters and data locations

After configuration, simply running

	make
	
should be sufficient to produce a `matscicut` executable that will do
all processing (it presently allows the dilation amount and slice
number to be specified as parameters).

## Files

For images, any type that OpenCV can read will be sufficient. For the
labels (segmentations), these are found in plain text label files,
where columns are delimited by spaces, and rows are newline
delimited. These can be read into MATLAB with `dlmread` and the
segmentation tools (seg2bmap.m readSeg.m) from the
[Berkeley Segmentation Benchmark](http://www.eecs.berkeley.edu/Research/Projects/CS/vision/bsds/)
can be used to display and process these segmentations.

## Status

This project is legacy code, and is completely unsupported. A new
implementation consisting of some of the components of this project is
already completed and (contingent upon its status) may be available
upon request.

---

Copyright 2011 Jarrell Waggoner. All rights reserved.

Jarrell Waggoner  
/-/ [malloc47.com](http://www.malloc47.com)
