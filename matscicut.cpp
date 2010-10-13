#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "GCoptimization.h"
// OpenCV
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

// #define INF 3162
#define INF 1000000

using namespace std;
using namespace cv;

int ind2subx(int ind, int w) {
	return ind % w;
}

int ind2suby(int ind, int x, int w) {
	return (ind - x) / w;
}

int sub2ind(int x, int y, int w) {
	return x+(y*w);
}

struct ForSmoothFn {
	int num_labels;
	int *data;
	int *adj;
	Mat img;
};

int smoothFn(int s1, int s2, int l1, int l2, void *extraData) {

	ForSmoothFn *extra = (ForSmoothFn *) extraData;
	int num_labels = extra->num_labels;
	int *data = extra->data;
	int *adj = extra->adj;
	Mat img = extra->img;

	if(l1 == l2) { return 0; }

	if(!adj[sub2ind(l1,l2,num_labels)]) { return INF; }

	int s1x = ind2subx(s1,num_labels);
	int s1y = ind2suby(s1,s1x,num_labels);
	int s2x = ind2subx(s2,num_labels);
	int s2y = ind2suby(s2,s2x,num_labels);

	int s1i = int(img.at<unsigned char>(s1x,s1y));
	int s2i = int(img.at<unsigned char>(s2x,s2y));

	if(abs(s1i-s2i) < 10) { return 10; }

//	cout << int(1.0/double(abs(s1i-s2i)+1) * INF) << endl;

	return int(1.0/double(abs(s1i-s2i)+1) * INF);
}

struct ForDataFn{
	int numLab;
	int *data;
};


int dataFn(int p, int l, void *data) {
	ForDataFn *myData = (ForDataFn *) data;
	int numLab = myData->numLab;
	
	return( myData->data[p*numLab+l] );
}



////////////////////////////////////////////////////////////////////////////////
// in this version, set data and smoothness terms using arrays
// grid neighborhood structure is assumed
//
void GridGraph_DfnSfn(int width,int height,int num_pixels,int num_labels) {

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
//		gc->setSmoothCost(&smoothFn);

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
void GridGraph_DArraySArraySpatVarying(int width,int height,int num_pixels,int num_labels) {
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

std::string ZeroPadNumber(int num,int pad) {
	std::ostringstream ss;
	ss << std::setw(pad) << std::setfill('0') << num;
	return ss.str();
}

int* loadRaw(string filename, int size) {
	cout << "Reading " << filename << endl;

	ifstream rawfilein;
	rawfilein.open(filename.c_str(),ios::in);

	if(!rawfilein) {
		cerr << "Can't open file: " << filename << endl;
		return 0;
	}

	int *raw = new int[size];

	for(int i=0;i<size;i++){
		int temp;
		rawfilein >> temp;	
		raw[i] = temp;
	}

	rawfilein.close();

	return raw;
	
}

void display(string handle, Mat img) {
	namedWindow(handle,CV_WINDOW_AUTOSIZE);
	imshow(handle,img);
	waitKey(0);
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "No argument" << std::endl;
		return 2;
	}

	// Read in basic files and information

	int framenum = atoi(argv[1]);

	string filea1="data/new/raw/4000_Series/4000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string filea2="data/new/raw/5000_Series/5000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string filea3="data/new/raw/6000_Series/6000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string filea4="data/new/raw/7000_Series/7000_image" + ZeroPadNumber(framenum,4) + ".tif";
	string fileb1="data/new/raw/4000_Series/4000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string fileb2="data/new/raw/5000_Series/5000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string fileb3="data/new/raw/6000_Series/6000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string fileb4="data/new/raw/7000_Series/7000_image" + ZeroPadNumber(framenum+1,4) + ".tif";
	string seedfile="data/new/ground/image" + ZeroPadNumber(framenum,4)+".pgm";

	cout << "Loading:" << endl << filea1 << endl << filea2 << endl << filea3 << endl << filea4 << endl << fileb1 << endl << fileb2 << endl << fileb3 << endl << fileb4 << endl << seedfile << endl; 


	Mat imga1 = imread(filea1,0);
	Mat seedimg = imread(seedfile,0);

	int width = imga1.size().width; 
	int height = imga1.size().height;
	int num_pixels = width*height;
	

	cout << "Image size: " << width  << "x" << height << endl;

	double templabels = 0;

	// Why this is fixed to doubles, I'll never know
	minMaxLoc(seedimg,NULL,&templabels,NULL,NULL);

	int num_labels = int(templabels)+1;

	cout << "Number of labels: " <<  num_labels << endl;

	int *adj = loadRaw("data/new/intermediate/image" + ZeroPadNumber(framenum,4)+".adj",num_labels*num_labels);
	int *data = loadRaw("data/new/intermediate/image" + ZeroPadNumber(framenum,4)+".data",num_pixels*num_labels);

	try {
		cout << "Initializing Grid Graph" << endl;
		
		// Setup grid-based graph
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,num_labels);

		// Use input data cost
		gc->setDataCost(data);

		// Setup data to pass to the smooth function
		ForSmoothFn toFn;
		toFn.data = data;
		toFn.adj = adj;
		toFn.num_labels = num_labels;
		toFn.img = imga1;

		// Send the smooth function pointer
		gc->setSmoothCost(&smoothFn,&toFn);

		cout << "Computing Expansion" << endl;
		
		cout << "Before optimization energy is " << gc->compute_energy() << endl;
		gc->expansion(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
		cout << "After optimization energy is " << gc->compute_energy() << endl;

		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

/*	namedWindow("image",CV_WINDOW_AUTOSIZE);
	imshow("image",imga1);
	waitKey(0);
*/
	display("image",imga1);
	delete [] data;
	delete [] adj;

	return 0;
}

