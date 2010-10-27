#include "matutil.h"
#include <getopt.h> // getopt_long
#include "GCoptimization.h"

// #define INF 3162
#define INF 10000000
//#define DILATE_AMOUNT 10 
#define N 255
#define LTHRESH 10
#define FNAMELEN 4

#define FORxy(a,b) for(int y=0;y<(a);y++) for(int x=0;x<(b);x++)

using namespace cv;
using namespace std;

const string datapath="data/old/scaled/";
//const string datapath="data/new/raw/";
const string outputpath="output2/";
//const string outputpath="output/"

struct ForSmoothFn {/*{{{*/
	int num_labels;
	Mat adj;
	int *sites;
};/*}}}*/

int smoothFn(int s1, int s2, int l1, int l2, void *extraData);
Mat regionsAdj(Mat regions, int num_regions);
vector<int> getAdj(Mat adj,int region);
vector<int> regionSizes(Mat regions);
Rect getWindow(vector<int> labels, Mat regions);
Mat selectRegion(Mat seedimg, int region);
int * globalDataTerm(Mat seedimg,int dilate_amount);
int * graphCut(int* data, int* sites, Mat seedimg, Mat adj);
Mat globalGraphCut(Mat img, Mat seedimg);
Mat localGraphCut(Mat img, Mat seedimg, int ptx, int pty, int boxsize);
Mat localGraphCut(Mat img, Mat seedimg);
