#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
// #include <fstream>
#include "GCoptimization.h"
#include <Magick++.h>

using namespace std;

struct ForDataFn{
	int numLab;
	int *data;
};


int smoothFn(int p1, int p2, int l1, int l2)
{
	if ( (l1-l2)*(l1-l2) <= 4 ) return((l1-l2)*(l1-l2));
	else return(4);
}

int dataFn(int p, int l, void *data)
{
	ForDataFn *myData = (ForDataFn *) data;
	int numLab = myData->numLab;
	
	return( myData->data[p*numLab+l] );
}



////////////////////////////////////////////////////////////////////////////////
// in this version, set data and smoothness terms using arrays
// grid neighborhood structure is assumed
//
void GridGraph_DfnSfn(int width,int height,int num_pixels,int num_labels)
{

	int *result = new int[num_pixels];   // stores result of optimization

	// first set up the array for data costs
	int *data = new int[num_pixels*num_labels];
	for ( int i = 0; i < num_pixels; i++ )
		for (int l = 0; l < num_labels; l++ )
			if (i < 25 ){
				if(  l == 0 ) data[i*num_labels+l] = 0;
				else data[i*num_labels+l] = 10;
			}
			else {
				if(  l == 5 ) data[i*num_labels+l] = 0;
				else data[i*num_labels+l] = 10;
			}


	try{
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,num_labels);

		// set up the needed data to pass to function for the data costs
		ForDataFn toFn;
		toFn.data = data;
		toFn.numLab = num_labels;

		gc->setDataCost(&dataFn,&toFn);

		// smoothness comes from function pointer
		gc->setSmoothCost(&smoothFn);

		printf("\nBefore optimization energy is %d",gc->compute_energy());
		gc->expansion(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
		printf("\nAfter optimization energy is %d",gc->compute_energy());

		for ( int  i = 0; i < num_pixels; i++ )
			result[i] = gc->whatLabel(i);

		delete gc;
	}
	catch (GCException e){
		e.Report();
	}

	delete [] result;
	delete [] data;

}
////////////////////////////////////////////////////////////////////////////////
// Uses spatially varying smoothness terms. That is 
// V(p1,p2,l1,l2) = w_{p1,p2}*[min((l1-l2)*(l1-l2),4)], with 
// w_{p1,p2} = p1+p2 if |p1-p2| == 1 and w_{p1,p2} = p1*p2 if |p1-p2| is not 1
void GridGraph_DArraySArraySpatVarying(int width,int height,int num_pixels,int num_labels)
{
	int *result = new int[num_pixels];   // stores result of optimization

	// first set up the array for data costs
	int *data = new int[num_pixels*num_labels];
	for ( int i = 0; i < num_pixels; i++ )
		for (int l = 0; l < num_labels; l++ )
			if (i < 25 ){
				if(  l == 0 ) data[i*num_labels+l] = 0;
				else data[i*num_labels+l] = 10;
			}
			else {
				if(  l == 5 ) data[i*num_labels+l] = 0;
				else data[i*num_labels+l] = 10;
			}
	// next set up the array for smooth costs
	int *smooth = new int[num_labels*num_labels];
	for ( int l1 = 0; l1 < num_labels; l1++ )
		for (int l2 = 0; l2 < num_labels; l2++ )
			smooth[l1+l2*num_labels] = (l1-l2)*(l1-l2) <= 4  ? (l1-l2)*(l1-l2):4;

	// next set up spatially varying arrays V and H

	int *V = new int[num_pixels];
	int *H = new int[num_pixels];

	
	for ( int i = 0; i < num_pixels; i++ ){
		H[i] = i+(i+1)%3;
		V[i] = i*(i+width)%7;
	}


	try{
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,num_labels);
		gc->setDataCost(data);
		gc->setSmoothCostVH(smooth,V,H);
		printf("\nBefore optimization energy is %d",gc->compute_energy());
		gc->expansion(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
		printf("\nAfter optimization energy is %d",gc->compute_energy());

		for ( int  i = 0; i < num_pixels; i++ )
			result[i] = gc->whatLabel(i);

		delete gc;
	}
	catch (GCException e){
		e.Report();
	}

	delete [] result;
	delete [] smooth;
	delete [] data;


}

std::string ZeroPadNumber(int num,int pad)
{
	std::ostringstream ss;
	ss << std::setw(pad) << std::setfill('0') << num;
	return ss.str();
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{

	if (argc < 2) {
		cerr << "No argument" << std::endl;
		return 2;
	}

	int framenum = atoi(argv[1]);

	string filea1="data/new/raw/4000_Series/4000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string filea2="data/new/raw/5000_Series/5000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string filea3="data/new/raw/6000_Series/6000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string filea4="data/new/raw/7000_Series/7000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string fileb1="data/new/raw/4000_Series/4000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string fileb2="data/new/raw/5000_Series/5000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string fileb3="data/new/raw/6000_Series/6000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string fileb4="data/new/raw/7000_Series/7000_image" + ZeroPadNumber(framenum+1,4) + ".tif";

	cout << "Loading:" << endl << filea1 << endl << filea2 << endl << filea3 << endl << filea4 << endl << fileb1 << endl << fileb2 << endl << fileb3 << endl << fileb4 << endl; 

	Magick::Image imga1, imga2, imga3, imga4, imgb1, imgb2, imgb3, imgb4;

	try {
		imga1.read(filea1.c_str());
		imga2.read(filea2.c_str());
		imga3.read(filea3.c_str());
		imga4.read(filea4.c_str());
		imgb1.read(fileb1.c_str());
		imgb2.read(fileb2.c_str());
		imgb3.read(fileb3.c_str());
		imgb4.read(fileb4.c_str());
	} catch (Magick::ErrorBlob &e) {
		cerr << "ERROR: Could not open file" << endl;
		return 2;
	}

	cout << "Image size: " << imga1.columns() << "x" << imga1.rows() << endl;
	
	int width = int(imga1.columns());
	int height = int(imga1.rows());
	int num_pixels = width*height;
	int num_labels = 7;

	// smoothness and data costs are set up using functions
	GridGraph_DfnSfn(width,height,num_pixels,num_labels);
	
	// smoothness and data costs are set up using arrays. 
	// spatially varying terms are present
	GridGraph_DArraySArraySpatVarying(width,height,num_pixels,num_labels);

	//printf("\n  Finished %d (%d) clock per sec %d",clock()/CLOCKS_PER_SEC,clock(),CLOCKS_PER_SEC);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////

