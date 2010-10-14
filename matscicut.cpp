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
#define INF 10000000

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
	Mat adj;
	Mat img;
};

int smoothFn(int s1, int s2, int l1, int l2, void *extraData) {

	return 0;

	ForSmoothFn *extra = (ForSmoothFn *) extraData;
	int num_labels = extra->num_labels;
	int *data = extra->data;
	Mat adj = extra->adj;
	Mat img = extra->img;

	if(l1 == l2) { return 0; }

//	if(!adj[sub2ind(l1,l2,num_labels)]) { return INF; }
	if(!int(adj.at<unsigned char>(l1,l2))) { return INF; }

	int s1x = ind2subx(s1,num_labels);
	int s1y = ind2suby(s1,s1x,num_labels);
	int s2x = ind2subx(s2,num_labels);
	int s2y = ind2suby(s2,s2x,num_labels);

	int s1i = int(img.at<unsigned char>(s1x,s1y));
	int s2i = int(img.at<unsigned char>(s2x,s2y));

	if(abs(s1i-s2i) < 10) { return 10; }

	return int(1.0/double(abs(s1i-s2i)+1) * INF);
}

std::string ZeroPadNumber(int num,int pad) {
	std::ostringstream ss;
	ss << std::setw(pad) << std::setfill('0') << num;
	return ss.str();
}

void writeRaw(string filename, int* data, int size) {
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

Mat regionsAdj(Mat regions, int num_regions) {
	
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
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}
	for(int x=1;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = int(regions.at<unsigned char>(x,y));
			int r2 = int(regions.at<unsigned char>(x-1,y));
			if( r1 != r2 ) {
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}
	for(int x=0;x<regions.size().width;x++) {
		for(int y=1;y<regions.size().height;y++) {
			int r1 = int(regions.at<unsigned char>(x,y));
			int r2 = int(regions.at<unsigned char>(x,y-1));
			if( r1 != r2 ) {
				adj.at<unsigned char>(r2,r1) = 1;
			}
		}
	}

	return adj;
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
	Mat imgb1 = imread(fileb1,0);
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

//	int *adj = loadRaw("data/new/intermediate/image" + ZeroPadNumber(framenum,4)+".adj",num_labels*num_labels);
//	int *data = loadRaw("data/new/intermediate/image" + ZeroPadNumber(framenum,4)+".data",num_pixels*num_labels);
	int *data = new int[num_pixels*num_labels];
	int *result = new int[num_pixels];   // stores result of optimization

	Mat adj = regionsAdj(seedimg,num_labels);

	for(int l=0;l<num_labels;l++) {
		for(int x=0;x<width;x++) {
			for(int y=0;y<height;y++) {
				data[ ( x+y*width ) * num_labels + l ] = (int(seedimg.at<unsigned char>(x,y)) == l ? 0 : INF);
			}
		}
	}

	for(int i=0;i<num_pixels*num_labels;i++)
		if(data[i] != 0 && data[i] != INF)
			cout << data[i] << endl; 

	try {
		cout << "Initializing Grid Graph" << endl;
		
		// Setup grid-based graph
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(width,height,num_labels);

/*		for(int i=0;i<width;i++) {
			for(int j=0;j<width;j++) {
				gc->setLabel(sub2ind(i,j,width),int(seedimg.at<unsigned char>(i,j)));
			}
		}
*/
		// Use input data cost
		gc->setDataCost(data);

		// Setup data to pass to the smooth function
		ForSmoothFn toFn;
		toFn.data = data;
		toFn.adj = adj;
		toFn.num_labels = num_labels;
		toFn.img = imgb1;

		// Send the smooth function pointer
		gc->setSmoothCost(&smoothFn,&toFn);

		cout << "Computing Expansion" << endl;
		
		cout << "Before optimization energy is " << gc->compute_energy() << endl;
		gc->expansion(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
//		gc->swap(2);// run expansion for 2 iterations. For swap use gc->swap(num_iterations);
		cout << "After optimization energy is " << gc->compute_energy() << endl;

		int *result = new int[num_pixels];   // stores result of optimization

		for ( int  i = 0; i < num_pixels; i++ ) {
			result[i] = gc->whatLabel(i);
		}

		writeRaw("image"+ZeroPadNumber(framenum,4)+".labels",result,num_pixels);

		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

	display("image",imgb1);
	delete [] data;
//	delete [] adj;
	delete [] result;

	return 0;
}

