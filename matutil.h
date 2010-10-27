#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

using namespace cv;
using namespace std;

int ind2subx(int ind, int w); 
int ind2suby(int ind, int x, int w);
int sub2ind(int x, int y, int w);
Mat loadMat(string filename, int width, int height);
void writeMat(string filename, Mat data);
string zpnum(int num,int pad);
void writeRaw(string filename, int* data, int size);
int* loadRaw(string filename, int size);
void display(string handle, Mat img);
void printstats (Mat img);
int mat_max(Mat matrix);
int * toLinear(Mat matrix);
Mat toMat(int* data,int width,int height);
Mat overlay(Mat img1, Mat img2,float alpha,int hilight=-1);
bool cmpMat(Mat a, Mat b);
Mat combine(Mat a, Mat b, float blend = 0.5);
template <typename T>
void swap(const T &a, const T &b);
void printVector(vector<int> vec);
