#include "matutil.h"
#include <getopt.h> // getopt_long
#include "GCoptimization.h"

// #define INF 3162
#define INF 10000000
//#define DILATE_AMOUNT 10 
#define N 255
#define LTHRESH 10
#define FNAMELEN 4
#define ADDWIN 30 
#define WINTHRESH 25 

#define FORxy(a,b) for(int y=0;y<(a);y++) for(int x=0;x<(b);x++)
#define FORxyM(m) for(int y=0;y<(m).size().height;y++) for(int x=0;x<(m).size().width;x++)

using namespace cv;
using namespace std;

const string datapath="data/old/scaled/";
//const string datapath="data/new/raw/";
const string outputpath="output2/";
//const string outputpath="output/"
const char bar[4] = {'\\', '|', '/','-'};
const float PI = std::atan(1.0)*4;

struct ForSmoothFn {/*{{{*/
	int num_labels;
	Mat adj;
	int *sites;
};/*}}}*/

int smoothFn(int s1, int s2, int l1, int l2, void *extraData);
Mat regionsAdj(Mat regions, int num_regions);
int regionNum(Mat regions);
Mat regionCompact(Mat regionsin);
Mat regionClean(Mat regionsin);
vector<int> getAdj(Mat adj,int region);
vector<int> regionSizes(Mat regions);
int regionSize(Mat regions,int region);
vector< vector<int> > junctionRegions(Mat regions);
Rect getWindow(vector<int> regions, Mat labels);
Mat selectRegion(Mat seedimg, int region);
Mat clearRegions(Mat seedimg, vector<int> regions);
int * globalDataTerm(Mat seedimg,int dilate_amount);
int * junctionDataTerm(Mat seedimg,Point center,vector<int> regions,Point seed);
int * graphCut(int* data, int* sites, Mat seedimg, Mat adj, int num_labels, bool initialize=true);
Mat globalGraphCut(Mat img, Mat seedimg);
Mat junctionGraphCut(Mat img, Mat seedimgin, Point center, vector<int> regions, Point seed);
vector< vector<int> > selectSeedPoints(Mat seedimg, Point center, int r);
Mat shiftSubregion(Mat seedimg, vector<int> regions);
Mat shiftBackSubregion(Mat seedimg, Mat newseedimg, vector<int> regions);
Mat processJunctions(Mat img, Mat seedimg);
Mat processDelete(Mat img, Mat seedimg);
