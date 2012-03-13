#include "matutil.h"
#include <getopt.h> // getopt_long
#include "GCoptimization.h"

// #define INF 3162
#define INF 10000000
//#define DILATE_AMOUNT 10 
#define N 255
#define LTHRESH 10
// Window sizes for junctions
#define ADDWIN 30 
#define WINTHRESH 25 
// Size limits for delete
#define DEL_UPPER 200
#define DEL_LOWER 50
#define DEL_DILATE 20
// Variables for edge processing
#define EDG_SIZE 300
#define EDG_LENGTH 4
#define EDG_DILATE 40
// Variables for junction processing
#define JNT_DILATE 20

// The percentage of pixels that should fall on high intensity edges
#define BORDER_CORRELATION 0.66

#define FORxy(a,b) for(int y=0;y<(a);y++) for(int x=0;x<(b);x++)
#define FORxyM(m) for(int y=0;y<(m).size().height;y++) for(int x=0;x<(m).size().width;x++)

using namespace cv;
using namespace std;

// #define FNAMELEN 4
// const string datapath="seq2/img/";
// //const string outputpath="output/";
// const string labelpath = "seq2/labels/";
// const string imgtype = "png";
// const string labeltype = "label";

#define FNAMELEN 4
//const string datapath="seq3/img/";
//const string labelpath = "seq3/labels/";
//const string imgtype = "png";
//const string labeltype = "label";
//string postfix = "";
//string prefix  = "image";

const string datapath="seq1/img/";
const string labelpath = "seq1/labels/";
const string imgtype = "png";
const string labeltype = "label";
string postfix = "";
string prefix  = "image";

const char bar[4] = {'\\', '|', '/','-'};
const float PI = std::atan(1.0)*4;

int meanthresh = 0;
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
Point regionCentroid(Mat seedimg, int region);
float regionBorderCriteria(Mat img, Mat regions, int region, int compregion);
vector<Point> regionEdge(Mat regions,int l1,int l2);
vector<int> getAdj(Mat adj,int region);
vector<int> regionSizes(Mat regions);
int regionSize(Mat regions,int region);
vector< vector<int> > junctionRegions(Mat regions);
Rect getWindow(vector<int> regions, Mat labels);
Mat selectRegion(Mat seedimg, int region);
Mat clearRegions(Mat seedimg, vector<int> regions);
void gaussian(Mat img, Mat mask, double& meanVal, double& stdDev);
bool fitGaussian(int val, int dist, double meanVal, double stdDev);
int * globalDataTerm(Mat img, Mat seedimg,int dilate_amount);
int * junctionDataTerm(Mat seedimg,Point center,vector<int> regions,Point seed);
int * graphCut(int* data, int* sites, Mat seedimg, Mat adj, int num_labels, bool initialize=true);
Mat globalGraphCut(Mat img, Mat seedimg);
Mat junctionGraphCut(Mat img, Mat seedimgin, Point center, vector<int> regions, Point seed);
vector< vector<int> > selectSeedPoints(Mat seedimg, Point center, int r);
Mat shiftSubregion(Mat seedimg, vector<int> regions);
Mat shiftBackSubregion(Mat seedimg, Mat newseedimg, vector<int> regions);
Mat processJunctions(Mat img, Mat seedimg);
Mat processDelete(Mat img, Mat seedimg);
