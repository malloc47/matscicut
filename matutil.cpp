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
	cout << "<readingMat \t" << filename << endl;


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
void writeMat(string filename, Mat data) {/*{{{*/
	cout << ">writingMat \t" << filename << endl;

	ofstream rawfileout;
	rawfileout.open (filename.c_str());

	if(!rawfileout) {
		cerr << ":can't open file: " << filename << endl;
		exit(1);
	}

	for(int x=0;x<data.size().width;x++) for(int y=0;y<data.size().height;y++) {
		rawfileout << data.at<int>(x,y) << " ";
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
}/*}}}*/
int mat_max(Mat matrix) {/*{{{*/
	double templabels = 0;
	minMaxLoc(matrix,NULL,&templabels,NULL,NULL);
	return int(templabels);
}/*}}}*/
int * toLinear(Mat matrix) {/*{{{*/
	int *linear = new int[matrix.size().width * matrix.size().height];
	for(int x=0;x<matrix.size().width;x++) for(int y=0;y<matrix.size().height;y++) 
		switch(matrix.type()) {
			case CV_8U:
				linear[sub2ind(x,y,matrix.size().width)] = int( matrix.at<unsigned char>(x,y) );
				break;
			case CV_32S:
				linear[sub2ind(x,y,matrix.size().width)] = matrix.at<int>(x,y);
				break;

		}
	return linear;
}/*}}}*/
Mat toMat(int* data,int width,int height) {/*{{{*/
	Mat output(width,height,CV_32S);
	int c=0;
	// Watch the ordering
	for(int y=0;y<height;y++) for(int x=0;x<width;x++)
		output.at<int>(x,y) = data[c++];
	return output;
}/*}}}*/
Mat overlay(Mat img1, Mat img2,float alpha) {/*{{{*/
	Mat composite(img1.size().width,img1.size().height,CV_8UC3,Scalar(0));

	int colormap[64][3] = {{0,0,143},/*{{{*/
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
		{128,0,0}};/*}}}*/

	Mat_<Vec3b>& img = (Mat_<Vec3b>&)composite;

	for(int x=0;x<img1.size().width;x++) for(int y=0;y<img1.size().height;y++) {
		img(x,y)[1] = int(alpha * colormap[img1.at<int>(x,y) % 64][1]) + int((1-alpha) * int(img2.at<unsigned char>(x,y))); 
		img(x,y)[2] = int(alpha * colormap[img1.at<int>(x,y) % 64][2]) + int((1-alpha) * int(img2.at<unsigned char>(x,y))); 
		img(x,y)[3] = int(alpha * colormap[img1.at<int>(x,y) % 64][3]) + int((1-alpha) * int(img2.at<unsigned char>(x,y))); 
	}

	return composite;

}/*}}}*/
bool cmpMat(Mat a, Mat b) {/*{{{*/
	if(a.size().width != b.size().width || a.size().height != b.size().height) {
		cout << "Comparing matrices of different sizes." << endl;
		return false;
	}

	for(int x=0;x<a.size().width;x++) for(int y=0;y<a.size().height;y++)
		if( a.at<int>(x,y) != b.at<int>(x,y) )
			return false;

	return true;


}/*}}}*/
