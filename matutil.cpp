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
	for(int y=0;y<height;y++) for(int x=0;x<width;x++)
		output.at<int>(x,y) = data[c++];
	return output;
}/*}}}*/
