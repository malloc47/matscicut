#include <stdio.h>/*{{{*/
#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include "GCoptimization.h"
// OpenCV
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

// #define INF 3162
#define INF 10000000
#define DILATE_AMOUNT 10 
#define N 255
#define LTHRESH 10
#define FNAMELEN 4

using namespace cv;
using namespace std;/*}}}*/

int ind2subx(int ind, int w) {/*{{{*/
	return ind % w;
}/*}}}*/

int ind2suby(int ind, int x, int w) {/*{{{*/
	return (ind - x) / w;
}/*}}}*/

int sub2ind(int x, int y, int w) {/*{{{*/
	return x+(y*w);
}/*}}}*/

struct ForSmoothFn {/*{{{*/
	int num_labels;
	Mat adj;
	int *sites;
};/*}}}*/

int smoothFn(int s1, int s2, int l1, int l2, void *extraData) {/*{{{*/

	ForSmoothFn *extra = (ForSmoothFn *) extraData;
	int num_labels = extra->num_labels;
	Mat adj = extra->adj;
	int *sites = extra->sites;

	if(l1 == l2) { return 0; }

	if(!int(adj.at<unsigned char>(l1,l2))) { return INF; }

	return int((1.0/double((abs(sites[s1]-sites[s2]) < LTHRESH ? LTHRESH : abs(sites[s1]-sites[s2]))+1)) * N);
}/*}}}*/

std::string ZeroPadNumber(int num,int pad) {/*{{{*/
	std::ostringstream ss;
	ss << std::setw(pad) << std::setfill('0') << num;
	return ss.str();
}/*}}}*/

void writeRaw(string filename, int* data, int size) {/*{{{*/
	cout << "Writing " << filename << endl;

	ofstream rawfileout;
	rawfileout.open (filename.c_str());

	if(!rawfileout) {
		cerr << "Can't open file: " << filename << endl;
		return;
	}

	for(int i=0;i<size;i++) {
		rawfileout << data[i] << " ";
	}

	rawfileout.close();
}/*}}}*/

int* loadRaw(string filename, int size) {/*{{{*/
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
	
}/*}}}*/

void display(string handle, Mat img) {/*{{{*/
	namedWindow(handle,CV_WINDOW_AUTOSIZE);
	imshow(handle,img);
	waitKey(0);
}/*}}}*/

Mat regionsAdj(Mat regions, int num_regions) {/*{{{*/
	Mat adj = Mat::zeros(num_regions,num_regions,CV_8U);

	for(int x=0;x<regions.size().width-1;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = int(regions.at<unsigned char>(x,y));
			int r2 = int(regions.at<unsigned char>(x+1,y));
			if( r1 != r2 ) {
				adj.at<unsigned char>(r1,r2) = 1;
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}

	for(int x=0;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height-1;y++) {
			int r1 = int(regions.at<unsigned char>(x,y));
			int r2 = int(regions.at<unsigned char>(x,y+1));
			if( r1 != r2 ) {
				adj.at<unsigned char>(r1,r2) = 1;
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}
	for(int x=1;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = int(regions.at<unsigned char>(x,y));
			int r2 = int(regions.at<unsigned char>(x-1,y));
			if( r1 != r2 ) {
				adj.at<unsigned char>(r1,r2) = 1;
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}
	for(int x=0;x<regions.size().width;x++) {
		for(int y=1;y<regions.size().height;y++) {
			int r1 = int(regions.at<unsigned char>(x,y));
			int r2 = int(regions.at<unsigned char>(x,y-1));
			if( r1 != r2 ) {
				adj.at<unsigned char>(r1,r2) = 1;
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}


	return adj;
}/*}}}*/

void printstats (Mat img) {/*{{{*/
	double mat_min = -1;
	double mat_max = -1;
	minMaxLoc(img,&mat_min,&mat_max,NULL,NULL);
	cout << "Min: " << mat_min << endl;
	cout << "Max: " << mat_max << endl;
}/*}}}*/

int mat_max(Mat matrix) {/*{{{*/
	double templabels = 0;
	minMaxLoc(matrix,NULL,&templabels,NULL,NULL);
	return int(templabels);
}/*}}}*/

int * toLinearIndex(Mat matrix) {/*{{{*/
	int *linear = new int[matrix.size().width * matrix.size().height];
	for(int x=0;x<matrix.size().width;x++) for(int y=0;y<matrix.size().height;y++) 
		linear[sub2ind(x,y,matrix.size().width)] = int( matrix.at<unsigned char>(x,y) );
	return linear;
}/*}}}*/

int * dataTerm(Mat seedimg) {/*{{{*/

	int num_pixels = seedimg.size().width*seedimg.size().height;

	int num_labels = mat_max(seedimg)+1;

	int *data = new int[num_pixels*num_labels];

	for(int l=0;l<num_labels;l++) {
		Mat layer = seedimg.clone();
		Mat dilation = seedimg.clone();
		Mat lut(256,1,CV_8U);
		for(int i=0;i<256;i++) lut.at<unsigned char>(i,0) = i==l ? 255 : 0; 
		LUT(seedimg,lut,layer); // Lookup table trick to zero out all but desired region
		cout << "." << flush;

		dilate(layer,dilation,getStructuringElement(MORPH_ELLIPSE,Size(DILATE_AMOUNT,DILATE_AMOUNT)));

		for(int x=0;x<seedimg.size().width;x++) for(int y=0;y<seedimg.size().height;y++) 
				data[ ( x+y*seedimg.size().width) * num_labels + l ] = (int(dilation.at<unsigned char>(x,y)) == 255 ? 0 : INF);
	}

	cout << endl;

	return data;

}/*}}}*/

int * graphCut(int* data, int* sites, Mat seedimg) {/*{{{*/

	int *result = new int[seedimg.size().width*seedimg.size().height];

	return result;
}/*}}}*/

int main(int argc, char **argv) {/*{{{*/
	int iterations;

	if (argc < 2) {
		cerr << "No argument" << std::endl;
		return 2;
	}

	if (argc < 3) 
		iterations = 1;
	else
		iterations = atoi(argv[2]);

	// Read in basic files and information

	int framenum = atoi(argv[1]);

	string filea1="data/new/raw/4000_Series/4000_image" + ZeroPadNumber(framenum,FNAMELEN) + ".tif";
	string filea2="data/new/raw/5000_Series/5000_image" + ZeroPadNumber(framenum,FNAMELEN) + ".tif";
	string filea3="data/new/raw/6000_Series/6000_image" + ZeroPadNumber(framenum,FNAMELEN) + ".tif";
	string filea4="data/new/raw/7000_Series/7000_image" + ZeroPadNumber(framenum,FNAMELEN) + ".tif";
	string fileb1="data/new/raw/4000_Series/4000_image" + ZeroPadNumber(framenum+1,FNAMELEN) + ".tif";
	string fileb2="data/new/raw/5000_Series/5000_image" + ZeroPadNumber(framenum+1,FNAMELEN) + ".tif";
	string fileb3="data/new/raw/6000_Series/6000_image" + ZeroPadNumber(framenum+1,FNAMELEN) + ".tif";
	string fileb4="data/new/raw/7000_Series/7000_image" + ZeroPadNumber(framenum+1,FNAMELEN) + ".tif";
	string seedfile="data/new/ground/image" + ZeroPadNumber(framenum,FNAMELEN)+".pgm";

//	cout << "Loading:" << endl << filea1 << endl << filea2 << endl << filea3 << endl << filea4 << endl << fileb1 << endl << fileb2 << endl << fileb3 << endl << fileb4 << endl << seedfile << endl; 

	Mat imga1 = imread(filea1,0);
	Mat imgb1 = imread(fileb1,0);
	Mat seedimg = imread(seedfile,0);

	// Shift from MATLAB's 1:N to 0:N-1
	seedimg -= 1;

	int width = imga1.size().width; 
	int height = imga1.size().height;
	int num_pixels = width*height;

	cout << "Image size: " << width  << "x" << height << endl;

	double templabels = 0;

	// Why this is fixed to doubles, I'll never know
	minMaxLoc(seedimg,NULL,&templabels,NULL,NULL);

	int num_labels = int(templabels)+1;

	cout << "Number of labels: " <<  num_labels << endl;

	int *result = new int[num_pixels];   // stores result of optimization

	Mat adj = regionsAdj(seedimg,num_labels);

	cout << "Computing data term" << flush;

	int *data = dataTerm(seedimg);

	cout << "Computing sites" << endl;

	int *sites = toLinearIndex(imgb1);

	try {
		cout << "Initializing Grid Graph" << endl;
		
		// Setup grid-based graph
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,num_labels);

		// Initialize labels
		for(int x=0;x<width;x++) {
			for(int y=0;y<width;y++) {
				gc->setLabel(sub2ind(x,y,width),int(seedimg.at<unsigned char>(x,y)));
			}
		}

		// Use input data cost
		gc->setDataCost(data);

		// Setup data to pass to the smooth function
		ForSmoothFn toFn;
		toFn.adj = adj;
		toFn.num_labels = num_labels;
		toFn.sites = sites;


		// Send the smooth function pointer
		gc->setSmoothCost(&smoothFn,&toFn);

		// Load current labeling into result
		for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);
		
		// Write out seed
		writeRaw("data/new/intermediate/image"+ZeroPadNumber(framenum+1,FNAMELEN)+"-"+ZeroPadNumber(0,2)+ ".labels",result,num_pixels);

		cout << "Computing Alpha-Beta Expansion" << endl;
	
		cout << "I: 0, T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

		// Loop for each iteration
		for(int iter=1; iter<=iterations; iter++) {

			gc->swap(1);

			for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);

			writeRaw("data/new/intermediate/image"+ZeroPadNumber(framenum+1,FNAMELEN)+"-"+ZeroPadNumber(iter,2)+ ".labels",result,num_pixels);

			cout << "I: " << iter << ", T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

		}

		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

	//	display("image",imgb1);
	delete [] data;
	delete [] sites;
	delete [] result;

	return 0;
}/*}}}*/
