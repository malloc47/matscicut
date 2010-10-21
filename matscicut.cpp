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
#include <getopt.h> // getopt_long
//#include <unistd.h> // getopt

// #define INF 3162
#define INF 10000000
//#define DILATE_AMOUNT 10 
#define N 255
#define LTHRESH 10
#define FNAMELEN 4

int dilate_amount = 10;

using namespace cv;
using namespace std;

const string datapath="data/new/raw/";
const string outputpath="output/";

/*}}}*/

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

	if(!adj.at<int>(l1,l2)) { return INF; }

	return int((1.0/double((abs(sites[s1]-sites[s2]) < LTHRESH ? LTHRESH : abs(sites[s1]-sites[s2]))+1)) * N);
}/*}}}*/

std::string zpnum(int num,int pad) {/*{{{*/
	std::ostringstream ss;
	ss << std::setw(pad) << std::setfill('0') << num;
	return ss.str();
}/*}}}*/

void writeRaw(string filename, int* data, int size) {/*{{{*/
	cout << ">writing \t" << filename << endl;

	ofstream rawfileout;
	rawfileout.open (filename.c_str());

	if(!rawfileout) {
		cerr << ":can't open file: " << filename << endl;
		exit(1);
	}

	for(int i=0;i<size;i++) {
		rawfileout << data[i] << " ";
	}

	rawfileout.close();
}/*}}}*/

int* loadRaw(string filename, int size) {/*{{{*/
	cout << "<reading \t" << filename << endl;

	ifstream rawfilein;
	rawfilein.open(filename.c_str(),ios::in);

	if(!rawfilein) {
		cerr << ":can't open file: " << filename << endl;
		exit(1);
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

Mat loadMat(string filename, int width, int height) {/*{{{*/
	cout << "<reading \t" << filename << endl;


	ifstream rawfilein;
	rawfilein.open(filename.c_str(),ios::in);

	Mat raw(width,height,CV_32S);

	if(!rawfilein) {
		cerr << ":can't open file: " << filename << endl;
		exit(1);
	}

	for(int x=0;x<width;x++) for(int y=0;y<height;y++) {
		int temp;
		rawfilein >> temp;
		raw.at<int>(x,y)=temp;
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
	//Mat adj = Mat::zeros(num_regions,num_regions,CV_8U);
	Mat adj(num_regions,num_regions,CV_32S);

	// Initialize to zero
	for(int i=0;i<num_regions;i++) for(int j=0;j<num_regions;j++)
		adj.at<int>(i,j)=0;

	for(int x=0;x<regions.size().width-1;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(x,y);
			int r2 = regions.at<int>(x+1,y);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
			}
		}
	}

	for(int x=0;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height-1;y++) {
			int r1 = regions.at<int>(x,y);
			int r2 = regions.at<int>(x,y+1);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
			}
		}
	}
	for(int x=1;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(x,y);
			int r2 = regions.at<int>(x-1,y);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
			}
		}
	}
	for(int x=0;x<regions.size().width;x++) {
		for(int y=1;y<regions.size().height;y++) {
			int r1 = regions.at<int>(x,y);
			int r2 = regions.at<int>(x,y-1);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
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

Mat selectRegion(Mat seedimg, int region) {/*{{{*/
	Mat layer(seedimg.size(),CV_8U); // Must use CV_8U for dilation
	for(int x=0;x<seedimg.size().width;x++) for(int y=0;y<seedimg.size().height;y++) 
		layer.at<unsigned char>(x,y) = (seedimg.at<int>(x,y) == region ? 255 : 0);
	return layer;
}/*}}}*/

int * dataTerm(Mat seedimg) {/*{{{*/

	cout << "@data term" << flush;

	char bar[4] = {'\\', '|', '/','-'};

	// Could probably do better than recomputing these
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int num_labels = mat_max(seedimg)+1;

	int *data = new int[num_pixels*num_labels];

	cout << "\t\\" << flush;

	for(int l=0;l<num_labels;l++) {
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		cout << "\b" << flush;
		cout << bar[l%4] << flush;

		dilate(layer,dilation,getStructuringElement(MORPH_ELLIPSE,Size(dilate_amount,dilate_amount)));

		for(int x=0;x<seedimg.size().width;x++) for(int y=0;y<seedimg.size().height;y++) 
				data[ ( x+y*seedimg.size().width) * num_labels + l ] = (int(dilation.at<unsigned char>(x,y)) == 255 ? 0 : INF);
	}
	cout << "\b" << flush;
	cout << "Done" << endl;

	return data;

}/*}}}*/

int * graphCut(int* data, int* sites, Mat seedimg, Mat adj) {/*{{{*/

	//int *result = new int[seedimg.size().width*seedimg.size().height];

	int num_labels = mat_max(seedimg)+1;
	int num_pixels = seedimg.size().width*seedimg.size().height;

	int *result = new int[num_pixels];

	try {
		cout << "@grid graph" << endl;
		
		// Setup grid-based graph
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(seedimg.size().width,seedimg.size().height,num_labels);

		// Initialize labels
		for(int x=0;x<seedimg.size().width;x++) {
			for(int y=0;y<seedimg.size().height;y++) {
				gc->setLabel(sub2ind(x,y,seedimg.size().width),seedimg.at<int>(x,y));
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

		// Initialize labeling to previous slice 
		for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);

		cout << "@alpha-beta expansion" << endl;
	
		cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

		gc->swap(1);

		// Retrieve labeling
		for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);

		//writeRaw(outputpath+"labels/image"+zpnum(framenum,FNAMELEN)+".labels",result,num_pixels);

		cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;


		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

	return result;
}/*}}}*/

int main(int argc, char **argv) {

	// Read in cmd line args

    int cmdargs;
    static struct option long_options[] = {
        {"dilate", 1, 0, 'd'},
        {"help", 0, 0, 'h'},
        {NULL, 0, NULL, 0}
    };
    int option_index = 0;
    while ((cmdargs = getopt_long(argc, argv, "d:h",
                 long_options, &option_index)) != -1) {
        switch (cmdargs) {
        case 'd':
			dilate_amount = atoi(optarg);
            break;
		case 'h':
			cout << "Usage: matscicut [OPTION] framenumber\n"
					"Label propagation for material science image segmentation\n\n"
					"Options\n"
				    "  -d, --dilate\t global dilation amount\n"
					" (-h) --help\t show this help (-h works with no other options\n";
			exit(0);
			break;
        case '?':
			exit(0);
            break;
        }
    }

	int framenum = 1;
    if (optind < argc) {
        while (optind < argc)
			framenum = atoi(argv[optind++]);
    }

	// Sanity checks on input

	if(framenum < 1) {
		cout << ":frame must be > 0" << endl;
		exit(1);
	}

	if(dilate_amount < 1) {
		cout << ":dilate must be > 0" << endl;
		exit(1);
	}

	cout << "input" << endl;
	cout << "-frame: \t" << framenum << endl;
	cout << "-dilate: \t" << dilate_amount << endl;

	string filea1=datapath + "4000_Series/4000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string filea2=datapath + "5000_Series/5000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string filea3=datapath + "6000_Series/6000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string filea4=datapath + "7000_Series/7000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string fileb1=datapath + "4000_Series/4000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string fileb2=datapath + "5000_Series/5000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string fileb3=datapath + "6000_Series/6000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string fileb4=datapath + "7000_Series/7000_image" + zpnum(framenum,FNAMELEN) + ".tif";

	string seedfile=outputpath + "labels/image" + zpnum(framenum-1,FNAMELEN)+".labels";

	cout << "<reading \t" << fileb1 << endl;
	Mat imgb1 = imread(fileb1,0);

	int width = imgb1.size().width; 
	int height = imgb1.size().height;
	int num_pixels = width*height;

	cout << "-image size: \t" << width  << "x" << height << endl;

	Mat seedimg = loadMat(seedfile,width,height);

	int num_labels = mat_max(seedimg)+1;

	if(num_labels < 2) {
		cout << "Must have > 1 label" << endl;
		exit(1);
	}

	cout << "-labels: \t" <<  num_labels << endl;

	Mat adj = regionsAdj(seedimg,num_labels);

	cout << "processing" << endl;

	int *data = dataTerm(seedimg);

	cout << "@sites" << endl;

	int *sites = toLinearIndex(imgb1);

	int *result = graphCut(data,sites,seedimg,adj);

	writeRaw(outputpath+"labels/image"+zpnum(framenum,FNAMELEN)+".labels",result,num_pixels);

	delete [] data;
	delete [] sites;
	delete [] result;

	return 0;
}
