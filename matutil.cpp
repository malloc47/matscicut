#include "matutil.h"
int ind2subx(int ind, int w) {/*{{{*/
	return ind % w;
}/*}}}*/
int ind2suby(int ind, int x, int w) {/*{{{*/
	return (ind - x) / w;
}/*}}}*/
int sub2ind(int x, int y, int w) {/*{{{*/
	return x+(y*w);
}/*}}}*/
Mat loadMat(string filename, int width, int height) {/*{{{*/
	// Read from MATLAB's dlmwrite
	cout << "<readingMat \t" << filename << endl;

	ifstream rawfilein;
	rawfilein.open(filename.c_str(),ios::in);

	Mat raw(height,width,CV_32S);

	if(!rawfilein) {
		cerr << ":can't open file: " << filename << endl;
		exit(1);
	}

	for(int y=0;y<height;y++) for(int x=0;x<width;x++) {
		int temp;
		rawfilein >> temp;
		raw.at<int>(y,x)=temp;
	}

	rawfilein.close();

	return raw;
	
}/*}}}*/
void writeMat(string filename, Mat data) {/*{{{*/
	// Can be read by dlmread in MATLAB
	cout << ">writingMat \t" << filename << endl;

	ofstream rawfileout;
	rawfileout.open (filename.c_str());

	if(!rawfileout) {
		cerr << ":can't open file: " << filename << endl;
		exit(1);
	}

	for(int y=0;y<data.size().height;y++) {
		for(int x=0;x<data.size().width;x++) {
			rawfileout << data.at<int>(y,x);
			if(x<data.size().width-1)
				rawfileout << " ";
		}
		if(y<data.size().height-1)
			rawfileout << endl;
	}

	rawfileout.close();
}/*}}}*/
std::string zpnum(int num,int pad) {/*{{{*/
	std::ostringstream ss;
	ss << std::setw(pad) << std::setfill('0') << num;
	return ss.str();
}/*}}}*/
void writeRaw(string filename, int* data, int size) {/*{{{*/
	cout << ">writingRaw \t" << filename << endl;

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
	cout << "<readingRaw \t" << filename << endl;

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
void display(string handle, Mat img) {/*{{{*/
	namedWindow(handle,CV_WINDOW_AUTOSIZE);
	imshow(handle,img);
	waitKey(0);
}/*}}}*/
void printstats (Mat img) {/*{{{*/
	double mat_min = -1;
	double mat_max = -1;
	minMaxLoc(img,&mat_min,&mat_max,NULL,NULL);
	cout << "Min: " << mat_min << endl;
	cout << "Max: " << mat_max << endl;
	cout << "Type:" << img.type() << endl;
	cout << "Chan:" << img.channels() << endl;
}/*}}}*/
int mat_max(Mat matrix) {/*{{{*/
	double templabels = 0;
	minMaxLoc(matrix,NULL,&templabels,NULL,NULL);
	return int(templabels);
}/*}}}*/
int mat_min(Mat matrix) {/*{{{*/
	double templabels = 0;
	minMaxLoc(matrix,&templabels,NULL,NULL,NULL);
	return int(templabels);
}/*}}}*/
int * toLinear(Mat matrix) {/*{{{*/
	//cout << matrix.size().width << "," << matrix.size().height << endl;
	int *linear = new int[matrix.size().width * matrix.size().height];
	for(int y=0;y<matrix.size().height;y++) for(int x=0;x<matrix.size().width;x++) 
		switch(matrix.type()) {
			case CV_8U:
				linear[sub2ind(x,y,matrix.size().width)] = int( matrix.at<unsigned char>(y,x) );
				break;
			case CV_32S:
				linear[sub2ind(x,y,matrix.size().width)] = matrix.at<int>(y,x);
				break;

		}

	return linear;
}/*}}}*/
Mat toMat(int* data,int width,int height) {/*{{{*/
	Mat output(height,width,CV_32S);
	int c=0;
	// Watch the ordering
	for(int y=0;y<height;y++) for(int x=0;x<width;x++)
		output.at<int>(y,x) = data[c++];
	return output;
}/*}}}*/
Mat overlay(Mat img1, Mat img2,float alpha,int hilight) {/*{{{*/
	Mat composite(img2.size(),CV_8UC3,Scalar(0));

	int colormap[128][3] = {{0,0,143},/*{{{*/
		{0,0,159},
		{0,0,175},
		{0,0,191},
		{0,0,207},
		{0,0,223},
		{0,0,239},
		{0,0,255},
		{0,16,255},
		{0,32,255},
		{0,48,255},
		{0,64,255},
		{0,80,255},
		{0,96,255},
		{0,112,255},
		{0,128,255},
		{0,143,255},
		{0,159,255},
		{0,175,255},
		{0,191,255},
		{0,207,255},
		{0,223,255},
		{0,239,255},
		{0,255,255},
		{16,255,239},
		{32,255,223},
		{48,255,207},
		{64,255,191},
		{80,255,175},
		{96,255,159},
		{112,255,143},
		{128,255,128},
		{143,255,112},
		{159,255,96},
		{175,255,80},
		{191,255,64},
		{207,255,48},
		{223,255,32},
		{239,255,16},
		{255,255,0},
		{255,239,0},
		{255,223,0},
		{255,207,0},
		{255,191,0},
		{255,175,0},
		{255,159,0},
		{255,143,0},
		{255,128,0},
		{255,112,0},
		{255,96,0},
		{255,80,0},
		{255,64,0},
		{255,48,0},
		{255,32,0},
		{255,16,0},
		{255,0,0},
		{239,0,0},
		{223,0,0},
		{207,0,0},
		{191,0,0},
		{175,0,0},
		{159,0,0},
		{143,0,0},
		{128,0,0},
		{0,0,159}, // Repeat
		{0,0,175},
		{0,0,191},
		{0,0,207},
		{0,0,223},
		{0,0,239},
		{0,0,255},
		{0,16,255},
		{0,32,255},
		{0,48,255},
		{0,64,255},
		{0,80,255},
		{0,96,255},
		{0,112,255},
		{0,128,255},
		{0,143,255},
		{0,159,255},
		{0,175,255},
		{0,191,255},
		{0,207,255},
		{0,223,255},
		{0,239,255},
		{0,255,255},
		{16,255,239},
		{32,255,223},
		{48,255,207},
		{64,255,191},
		{80,255,175},
		{96,255,159},
		{112,255,143},
		{128,255,128},
		{143,255,112},
		{159,255,96},
		{175,255,80},
		{191,255,64},
		{207,255,48},
		{223,255,32},
		{239,255,16},
		{255,255,0},
		{255,239,0},
		{255,223,0},
		{255,207,0},
		{255,191,0},
		{255,175,0},
		{255,159,0},
		{255,143,0},
		{255,128,0},
		{255,112,0},
		{255,96,0},
		{255,80,0},
		{255,64,0},
		{255,48,0},
		{255,32,0},
		{255,16,0},
		{255,0,0},
		{239,0,0},
		{223,0,0},
		{207,0,0},
		{191,0,0},
		{175,0,0},
		{159,0,0},
		{143,0,0},
		{128,0,0}};/*}}}*/

	int randmap[128] = {
    54,6,46,17,35,63,1,23,28,27,49,2,13,21,14,39,30,47,45,4,26,5,57,61,55,52,32,34,41,18,12,25,53,56,22,19,48,9,37,8,60,36,64,16,3,58,44,15,7,10,20,24,40,11,43,50,38,51,59,29,62,31,33,42,54,6,46,17,35,63,1,23,28,27,49,2,13,21,14,39,30,47,45,4,26,5,57,61,55,52,32,34,41,18,12,25,53,56,22,19,48,9,37,8,60,36,64,16,3,58,44,15,7,10,20,24,40,11,43,50,38,51,59,29,62,31,33,42};

	Mat_<Vec3b>& img = (Mat_<Vec3b>&)composite;

	for(int y=0;y<img1.size().height;y++) for(int x=0;x<img1.size().width;x++) {
		if(img1.at<int>(y,x)==hilight) {
			img(y,x)[1] = 255;
			img(y,x)[2] = 255;
			img(y,x)[3] = 255;
		}
		else {
			img(y,x)[1] = int(alpha * colormap[randmap[img1.at<int>(y,x) % 128]][1]) + int((1-alpha) * int(img2.at<unsigned char>(y,x))); 
			img(y,x)[2] = int(alpha * colormap[randmap[img1.at<int>(y,x) % 128]][2]) + int((1-alpha) * int(img2.at<unsigned char>(y,x))); 
			img(y,x)[3] = int(alpha * colormap[randmap[img1.at<int>(y,x) % 128]][3]) + int((1-alpha) * int(img2.at<unsigned char>(y,x))); 
		}
	}

	return composite;

}/*}}}*/
bool cmpMat(Mat a, Mat b) {/*{{{*/
	if(a.size().width != b.size().width || a.size().height != b.size().height) {
		cout << "Comparing matrices of different sizes." << endl;
		return false;
	}
	
	if(a.type() == CV_32S)
		for(int x=0;x<a.size().width;x++) for(int y=0;y<a.size().height;y++)
			if( a.at<int>(y,x) != b.at<int>(y,x) )
				return false;

	if(a.type() == CV_8U)
		for(int x=0;x<a.size().width;x++) for(int y=0;y<a.size().height;y++)
			if( int(a.at<unsigned char>(y,x)) != int(b.at<unsigned char>(y,x)) )
				return false;

	if(a.type() != CV_32S && a.type() != CV_8U) {
		cout << "Bad type" << endl;
		return false;
	}

	return true;


}/*}}}*/
Mat combine(Mat a, Mat b, float blend) {/*{{{*/

	Mat c = a.clone();
	
	for(int y=0;y<a.size().height;y++) for(int x=0;x<a.size().width;x++)
		c.at<unsigned char>(y,x) = int( blend*float(a.at<unsigned char>(y,x)) + (1-blend)*float(b.at<unsigned char>(y,x)) );

	return c;

}/*}}}*/
template <typename T>/*{{{*/
void swap(const T &a, const T &b) {
	T temp;
	temp = a;
	a = b;
	b = temp;
}/*}}}*/
void printVector(vector<int> vec) {/*{{{*/
	for(int i=0;i<vec.size();i++)
		cout << vec.at(i) << " ";
	cout << endl;
}/*}}}*/
float euclideanDist(Point a, Point b) {/*{{{*/
	return sqrt(float(pow(a.x-b.x,2)+pow(a.y-b.y,2)));
}/*}}}*/
Point pointDirection(Point center, Point direction, int r) {/*{{{*/
	Point bearing = direction - center;
	float theta = atan2(bearing.y,bearing.x);
	return Point(r*cos(theta)+center.x,r*sin(theta)+center.y);
}/*}}}*/
