#include "matscicut.h"
#include <time.h>
#include "BlobResult.h"
int smoothFn(int s1, int s2, int l1, int l2, void *extraData) {/*{{{*/
	ForSmoothFn *extra = (ForSmoothFn *) extraData;
	int num_labels = extra->num_labels;
	Mat adj = extra->adj;
	int *sites = extra->sites;

	if(l1 == l2) { return 0; }

	if(!adj.at<int>(l1,l2)) { return INF; }
	
	//return int((1.0/double((abs(sites[s1]-sites[s2]) < LTHRESH ? LTHRESH : abs(sites[s1]-sites[s2]))+1)) * N);
	//return int( 1/(double(sites[s1]+sites[s2])/2) * N );
	//return int( N - int(double(sites[s1]+sites[s2])/2));
	return int( 1/(max(double(sites[s1]),double(sites[s2]))+1) * N );
	//return int( 1/(min(double(sites[s1]),double(sites[s2]))+1) * N );
}/*}}}*/
Mat regionsAdj(Mat regions, int num_regions) {/*{{{*/
	Mat adj(num_regions,num_regions,CV_32S);

	// Initialize to zero
	for(int i=0;i<num_regions;i++) for(int j=0;j<num_regions;j++)
		if(boundsCheck(adj,Point(j,i)))
			adj.at<int>(i,j)=0;

	// Horizontal, including first, exluding last column
	for(int x=0;x<regions.size().width-1;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x+1);
			if( r1 != r2 ) {
				if(boundsCheck(adj,Point(r2,r1)))
					adj.at<int>(r1,r2) = 1;
				if(boundsCheck(adj,Point(r1,r2)))
					adj.at<int>(r2,r1) = 1;
			}
		}
	}

	for(int x=0;x<regions.size().width-2;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x+2);
			if( r1 != r2 ) {
				if(boundsCheck(adj,Point(r2,r1)))
					adj.at<int>(r1,r2) = 1;
				if(boundsCheck(adj,Point(r1,r2)))
					adj.at<int>(r2,r1) = 1;
			}
		}
	}

	// Verticle, including first, excluding last row 
	for(int x=0;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height-1;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y+1,x);
			if( r1 != r2 ) {
				if(boundsCheck(adj,Point(r2,r1)))
					adj.at<int>(r1,r2) = 1;
				if(boundsCheck(adj,Point(r1,r2)))
					adj.at<int>(r2,r1) = 1;
			}
		}
	}
	
	for(int x=0;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height-2;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y+2,x);
			if( r1 != r2 ) {
				if(boundsCheck(adj,Point(r2,r1)))
					adj.at<int>(r1,r2) = 1;
				if(boundsCheck(adj,Point(r1,r2)))
					adj.at<int>(r2,r1) = 1;
			}
		}
	}
	
	// Horizontal, excluding first, including last column
	for(int x=1;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x-1);
			if( r1 != r2 ) {
				if(boundsCheck(adj,Point(r2,r1)))
					adj.at<int>(r1,r2) = 1;
				if(boundsCheck(adj,Point(r1,r2)))
					adj.at<int>(r2,r1) = 1;
			}
		}
	}

	// Verticle, excluding first, including last row 
	for(int x=0;x<regions.size().width;x++) {
		for(int y=1;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y-1,x);
			if( r1 != r2 ) {
				if(boundsCheck(adj,Point(r2,r1)))
					adj.at<int>(r1,r2) = 1;
				if(boundsCheck(adj,Point(r1,r2)))
					adj.at<int>(r2,r1) = 1;
			}
		}
	}

	return adj;
}/*}}}*/
int regionNum(Mat regions) {/*{{{*/
	set<int> unique_regions;
	//int count=0;
	//int max_regions=mat_max(regions);

	//vector<int> regioncount(mat_max(regions)+1,0);

	FORxyM(regions) {
		unique_regions.insert(regions.at<int>(y,x));
		//regioncount.at(regions.at<int>(y,x))++;
	}

	//for(int i=0;i<regioncount.size();i++)
		//if(regioncount.at(i) > 0)
			//count++;
	
	return unique_regions.size();

}/*}}}*/
Mat regionCompact(Mat regionsin) {/*{{{*/
	Mat regions = regionsin.clone();
	bool clean=false;
	while(!clean) {
		clean=true;
		for(int i=0;i<mat_max(regions)+1;i++) { // For each reg
			if(regionSize(regions,i)<1) { // If it's too small
				FORxyM(regions) { // Visit all regs
					if(regions.at<int>(y,x)>i) { // If they're bigger than the current reg
						if(boundsCheck(regions,Point(x,y))) {
							regions.at<int>(y,x)--; // Decrement by one
							clean=false;
						}
					}
				}
				if(!clean) break;
			}
		}
	}
	return regions;

}/*}}}*/
Mat regionClean(Mat regionsin) {/*{{{*/

	Mat regions = regionsin.clone();
	// Filter single pixel errors
	// Doesn't handle border, currently  
	for(int y=1;y<regions.size().height-1;y++) for(int x=1;x<regions.size().width-1;x++) {
		if( (regions.at<int>(y,x) != regions.at<int>(y,x-1) &&
		regions.at<int>(y,x) != regions.at<int>(y,x+1) ) ||
		( regions.at<int>(y,x) != regions.at<int>(y-1,x) &&
		regions.at<int>(y,x) != regions.at<int>(y+1,x) ) )
			regions.at<int>(y,x)=regions.at<int>(y,x-1); // Closest region
	}

	regions = regionCompact(regions);

	Mat regions_out(regions.size(),CV_32S,-1);

	for(int l=0;l<mat_max(regions)+1;l++) {

		Mat layer = selectRegion(regions,l).clone();


		IplImage oldlayer = layer;
		IplImage* blobimg = 0;

		CBlobResult blobs;
		blobs = CBlobResult(&oldlayer,NULL,0);

		CBlob blob;
		blobs.GetNthBlob(CBlobGetArea(),0,blob);

		blobimg = cvCreateImage(cvGetSize(&oldlayer),IPL_DEPTH_8U,3);
		FORxyM(layer) {
			CvScalar s;
			s.val[0] = 0;
			s.val[1] = 0;
			s.val[2] = 0;
			cvSet2D(blobimg,y,x,s);
		}

		blob.FillBlob(blobimg,CV_RGB(255,255,255));

		Mat new_layers = cvarrToMat(blobimg).clone();

	    vector<Mat> planes;
	    split(new_layers, planes);

		Mat new_layer = planes[0];

		cvReleaseImage(&blobimg);

		FORxyM(regions_out) 
			if(int(new_layer.at<unsigned char>(y,x)) > 0)
				if(boundsCheck(regions_out,Point(x,y)))
					regions_out.at<int>(y,x) = l;
		
	}


	while(mat_min(regions_out) < 0) {
		cout << "Cleaning" << endl;
		FORxyM(regions_out) {
			if(regions_out.at<int>(y,x) < 0) {
				if(x!=0 && regions_out.at<int>(y,x-1) != -1) {
					if(boundsCheck(regions_out,Point(x,y)))
						regions_out.at<int>(y,x) = regions_out.at<int>(y,x-1);
					continue;
				}
				if(y!=0 && regions_out.at<int>(y-1,x) != -1) {
					if(boundsCheck(regions_out,Point(x,y)))
						regions_out.at<int>(y,x) = regions_out.at<int>(y-1,x);
					continue;
				}
				if(x!=regions_out.size().width-1 && regions_out.at<int>(y,x+1) != -1) {
					if(boundsCheck(regions_out,Point(x,y)))
						regions_out.at<int>(y,x) = regions_out.at<int>(y,x+1);
					continue;
				}
				if(y!=regions_out.size().height-1 && regions_out.at<int>(y+1,x) != -1) {
					if(boundsCheck(regions_out,Point(x,y)))
						regions_out.at<int>(y,x) = regions_out.at<int>(y+1,x);
					continue;
				}
			}
		}
	}

	return regions_out;
}/*}}}*/
Point regionCentroid(Mat seedimg, int region) {/*{{{*/
	int centroidx=0;
	int centroidy=0;
	int centroidk=0;

	FORxyM(seedimg)
		if(seedimg.at<int>(y,x) == region) {
			centroidx += x;
			centroidy += y;
			centroidk++;
		}

	if(centroidk > 0) {
		centroidx /= centroidk;
		centroidy /= centroidk;
	}
	else {
		centroidx = 0;
		centroidy = 0;
	}

	return Point(centroidx,centroidy);

}/*}}}*/
Point vectorCentroid(vector<Point> edge) {/*{{{*/
	int centroidx=0;
	int centroidy=0;
	int centroidk=0;

	for(int i=0;i<edge.size();i++){
		centroidx += edge.at(i).x;
		centroidy += edge.at(i).y;
		centroidk++;
	}

	if(centroidk > 0) {
		centroidx /= centroidk;
		centroidy /= centroidk;
	}
	else {
		centroidx = 0;
		centroidy = 0;
	}

	return Point(centroidx,centroidy);

}/*}}}*/
/*bool regionBorderCriteria(Mat img, Mat regions, int region, float thresh) {[>{{{<]
	float total_border=0.0,total_thresh=0.0;
	FORxyM(regions) {	
		if(regions.at<int>(y,x) != region) continue;
		// Skip regions that are internal
		if( ( x!=0 && regions.at<int>(y,x)!=regions.at<int>(y,x-1) ) || 
		   (y!=0 && regions.at<int>(y,x)!=regions.at<int>(y-1,x) ) ||
		   (x!=regions.size().width-1 && regions.at<int>(y,x)!=regions.at<int>(y,x+1) ) ||
		   (y!=regions.size().height-1 && regions.at<int>(y,x)!=regions.at<int>(y+1,x)) ) {
			total_border++;
			if( int(img.at<unsigned char>(y,x) > 32) )
				total_thresh++;
		}
	}

	//cout << (total_thresh/total_border) << " > " << thresh << endl;

	return (total_thresh/total_border> thresh);
	
}[>}}}<]*/
float regionBorderCriteria(Mat img, Mat regions, int region, int compregion) {/*{{{*/
	float total_border=0.0,total_thresh=0.0;
	const int thresh = int(double(meanthresh)*0.75);
	
	// Horizontal, including first, exluding last column
	for(int x=0;x<regions.size().width-1;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x+1);
			if( r1==region && r2==compregion) {
				if(int(img.at<unsigned char>(y,x))>thresh)
					total_thresh++;
				total_border++;
			}
		}
	}

	// Verticle, including first, excluding last row 
	for(int x=0;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height-1;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y+1,x);
			if( r1==region && r2==compregion) {
				if(int(img.at<unsigned char>(y,x))>thresh)
					total_thresh++;
				total_border++;
			}
		}
	}
	
	// Horizontal, excluding first, including last column
	for(int x=1;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x-1);
			if( r1==region && r2==compregion) {
				if(int(img.at<unsigned char>(y,x))>thresh)
					total_thresh++;
				total_border++;
			}
		}
	}

	// Verticle, excluding first, including last row 
	for(int x=0;x<regions.size().width;x++) {
		for(int y=1;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y-1,x);
			if( r1==region && r2==compregion) {
				if(int(img.at<unsigned char>(y,x))>thresh)
					total_thresh++;
				total_border++;
			}
		}
	}

	return (total_thresh/total_border);
	
}/*}}}*/
vector<Point> regionEdge(Mat regions,int l1,int l2) {/*{{{*/
	vector<Point> points;

	if(!(l2 < 0)) {
		// Horizontal, including first, exluding last column
		for(int x=0;x<regions.size().width-1;x++) {
			for(int y=0;y<regions.size().height;y++) {
				int r1 = regions.at<int>(y,x);
				int r2 = regions.at<int>(y,x+1);
				if( (r1 == l1 && r2 == l2) ||
						(r1 == l2 && r2 == l1) )
					points.push_back(Point(x,y));
			}
		}

		// Verticle, including first, excluding last row 
		for(int x=0;x<regions.size().width;x++) {
			for(int y=0;y<regions.size().height-1;y++) {
				int r1 = regions.at<int>(y,x);
				int r2 = regions.at<int>(y+1,x);
				if( (r1 == l1 && r2 == l2) ||
						(r1 == l2 && r2 == l1) )
					points.push_back(Point(x,y));
			}
		}
	}
	else
		switch(l2) {
			case -1:
				for(int x=0;x<regions.size().width;x++) {
					int r1 = regions.at<int>(0,x);
					if(r1==l1) points.push_back(Point(x,0));
				}
				break;

			case -2:
				for(int x=0;x<regions.size().width;x++) {
					int r1 = regions.at<int>(regions.size().height-1,x);
					if(r1==l1) points.push_back(Point(x,regions.size().height-1));
				}
				break;
				
			case -3:
				for(int y=0;y<regions.size().height;y++) {
					int r1 = regions.at<int>(y,0);
					if(r1==l1) points.push_back(Point(0,y));
				}
				break;
				
			case -4:
				for(int y=0;y<regions.size().height;y++) {
					int r1 = regions.at<int>(y,regions.size().width-1);
					if(r1==l1) points.push_back(Point(regions.size().width-1,y));
				}
				break;
		}

	return points;

}/*}}}*/
vector<int> getAdj(Mat adj,int region) {/*{{{*/
	// Adj is symmetrical, so it shouldn't matter how we iterate through it
	vector<int> adjacent;
	for(int i=0;i<adj.size().width;i++) 
		if(adj.at<int>(region,i)==1 && i != region) {
			adjacent.push_back(i);
		}
	return adjacent;
}/*}}}*/
vector<int> regionSizes(Mat regions) {/*{{{*/
	int num_regions = mat_max(regions)+1;
	vector<int> sizes(num_regions,0);
	FORxyM(regions) {
		sizes.at(regions.at<int>(y,x))++;
		//sizes.at(regions.at<int>(y,x)) = sizes.at(regions.at<int>(y,x))+1;
	}
	return sizes;
}/*}}}*/
int regionSize(Mat regions,int region) {/*{{{*/
	int total=0;
	FORxyM(regions)
		total += regions.at<int>(y,x) == region ? 1 : 0;
	return total;
}/*}}}*/
vector< vector<int> > junctionRegions(Mat regions) {/*{{{*/
	// Stores x,y at junctions[i][0..1] and the
	// remaining three regions in junctions[i][2..4]
	time_t start,end;
	double dif;
	time(&start);
	vector< vector<int> > junctions;

	for(int y=0;y<regions.size().height-1;y++) for(int x=0;x<regions.size().width-1;x++) {
		set<int> win;

		win.insert(regions.at<int>(y,x));
		win.insert(regions.at<int>(y,x+1));
		win.insert(regions.at<int>(y+1,x));
		win.insert(regions.at<int>(y+1,x+1));

		if(win.size() == 3) {
			vector<int> entry(5,-1);
			entry[0]=x;
			entry[1]=y;
			int counter = 2;
			// Annoyingly, only easy access w/o boost is with iterator
			for (set<int>::iterator it=win.begin() ; it != win.end(); it++ )
				entry[counter++] = *it;
			junctions.push_back(entry);
		}
	}

	time(&end);

	cout << endl << "# " << difftime(end,start) << endl;

	return junctions;
		
}/*}}}*/
Rect getWindow(vector<int> regions, Mat labels) {/*{{{*/
	// Get bounding window around multiple regions
	int x0=labels.size().width, y0=labels.size().height, x1=0, y1=0;

	if(regions.size() < 1) {
		cout << "No regions to generate window" << endl;
		exit(0);
	}

	FORxyM(labels) 
		for(int z=0;z<regions.size();z++) 
			if(labels.at<int>(y,x) == regions.at(z)) {
				x0=min(x0,x);
				y0=min(y0,y);
				x1=max(x1,x);
				y1=max(y1,y);
			}

	return Rect(x0,y0,x1-x0+1,y1-y0+1);

}/*}}}*/
/*Rect getWindow(pair<int,int> regionpair, Mat labels) {[>{{{<]
	vector<int> regions;
	if(!(regionpair.first < 0)) regions.push_back(regionpair.first);
	if(!(regionpair.second < 0))regions.push_back(regionpair.second);
	if(regions.empty()) {cout << "Empty region!" << endl; exit(0);}
	return getWindow(regions,labels);
}[>}}}<]*/
Mat selectRegion(Mat seedimg, int region) {/*{{{*/
	Mat layer(seedimg.size(),CV_8U); // Must use CV_8U for dilation
	for(int y=0;y<seedimg.size().height;y++) for(int x=0;x<seedimg.size().width;x++) 
		if(boundsCheck(layer,Point(x,y)))
			layer.at<unsigned char>(y,x) = (seedimg.at<int>(y,x) == region ? 255 : 0);
	return layer;
}/*}}}*/
Mat clearRegions(Mat seedimg, vector<int> regions) {/*{{{*/	
	FORxyM(seedimg) {
		bool inRegion = false;
		for(int z=0;z<regions.size();z++)
			if(seedimg.at<int>(y,x) == regions[z]) inRegion=true;
		if(!inRegion) 
			if(boundsCheck(seedimg,Point(x,y)))
				seedimg.at<int>(y,x) = -1;
	}
	return seedimg;
}/*}}}*/
int * globalDataTerm(Mat seedimg,int dilate_amount) {/*{{{*/

	cout << "@data term" << flush;

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

		// Don't bother if no dilation
		if(dilate_amount > 0)
			dilate(layer,dilation,getStructuringElement(MORPH_ELLIPSE,Size(dilate_amount,dilate_amount)));

		//display("tmp",dilation);

		for(int y=0;y<seedimg.size().height;y++) for(int x=0;x<seedimg.size().width;x++) 
			data[ ( x+y*seedimg.size().width) * num_labels + l ] = (int(dilation.at<unsigned char>(y,x)) == 255 ? 0 : INF);
	}
	cout << "\b" << flush;
	cout << "Done" << endl;

	return data;

}/*}}}*/
int * junctionDataTerm(Mat seedimg,Point center,vector<int> regions,Point seed, int dilate_amount) {/*{{{*/
	int num_labels = regions.size()+2;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int *data = new int[num_pixels*num_labels];

	vector<Point> centroids(regions.size());
	for(int i=0;i<regions.size();i++)
		centroids.at(i) = regionCentroid(seedimg,i+1);

	// Create zero region first
	FORxyM(seedimg)
		if(seedimg.at<int>(y,x) == 0)
			data[(x+y*seedimg.size().width)*num_labels+0] = 0;
		else
			data[(x+y*seedimg.size().width)*num_labels+0] = INF;

	// Loop over all intermediate regions
	for(int k=0;k<regions.size();k++) {
		int l=k+1; // Labels start at 1, not zero, since we've already done the zero region
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		
		vector<Point> offlimits;
		offlimits.push_back(seed);
		for(int i=0;i<regions.size();i++)
			if(i!=k) // Allow our own seed to be included	
				offlimits.push_back(centroids.at(i));

		// Dilate region
		if(dilate_amount > 0)
			dilate(layer,dilation,getStructuringElement(MORPH_RECT,Size(dilate_amount,dilate_amount)));
		FORxyM(seedimg) {
			//Set to inf by default
			data[(x+y*seedimg.size().width)*num_labels+l] = INF;
			bool zero=true;
			// If outside the dilation, always INF
			if(int(dilation.at<unsigned char>(y,x)) != 255) continue;
			// If outside the admissible region (e.g. in the zero region) , always INF
			if(seedimg.at<int>(y,x) == 0) continue;
			// If in another region's seed, or the new seed, always INF
			for(int i=0;i<offlimits.size();i++)
				if(x==offlimits.at(i).x && y==offlimits.at(i).y)
					zero=false;
			if(zero)
				data[(x+y*seedimg.size().width)*num_labels+l] = 0;
		}
	}

	Rect win(center.x-ADDWIN,center.y-ADDWIN,ADDWIN*2,ADDWIN*2);

	FORxyM(seedimg) {
		// Always set seed, regardless of whether it is within the dilation or not
		if(x==seed.x && y==seed.y)
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = 0; 
		else
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = INF; 
		
		vector<Point> offlimits;
		for(int i=0;i<centroids.size();i++)
			offlimits.push_back(centroids.at(i));

		bool zero=true;
		if(!win.contains(Point(x,y))) continue;
		if(seedimg.at<int>(y,x) == 0) continue;
		for(int i=0;i<offlimits.size();i++)
			if(x==offlimits.at(i).x && y==offlimits.at(i).y)
				zero=false;
		if(zero)
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = 0;
	}

	return data;
}	/*}}}*/
int * edgeDataTerm(Mat seedimg,Mat edge,vector<int> regions,Point seed,int dilate_amount) {/*{{{*/
	vector<Point> seeds;
	seeds.push_back(seed);
	seeds.push_back(Point(seed.x+1,seed.y));
	seeds.push_back(Point(seed.x,seed.y+1));
	seeds.push_back(Point(seed.x+1,seed.y+1));

	int num_labels = regions.size()+2;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int *data = new int[num_pixels*num_labels];

	vector<Point> centroids(regions.size());
	for(int i=0;i<regions.size();i++)
		centroids.at(i) = regionCentroid(seedimg,i+1);

	// Create zero region first
	FORxyM(seedimg)
		if(seedimg.at<int>(y,x) == 0)
			data[(x+y*seedimg.size().width)*num_labels+0] = 0;
		else
			data[(x+y*seedimg.size().width)*num_labels+0] = INF;

	// Loop over all intermediate regions
	for(int i=0;i<regions.size();i++) {
		int l=i+1; // Labels start at 1, not zero, since we've already done the zero region
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		
		vector<Point> offlimits;
		for(int j=0;j<seeds.size();j++) 
			offlimits.push_back(seeds.at(j));
		//offlimits.push_back(seed);
		//offlimits.push_back(Point(seed.x+1,seed.y));
		//offlimits.push_back(Point(seed.x,seed.y+1));
		//offlimits.push_back(Point(seed.x+1,seed.y+1));
		for(int j=0;j<regions.size();j++)
			if(i!=j) // Allow our own seed to be included	
				offlimits.push_back(centroids.at(j));

		// Dilate region
		if(dilate_amount > 0)
			dilate(layer,dilation,getStructuringElement(MORPH_RECT,Size(dilate_amount,dilate_amount)));
		FORxyM(seedimg) {
			//Set to inf by default
			data[(x+y*seedimg.size().width)*num_labels+l] = INF;
			bool zero=true;
			// If outside the dilation, always INF
			if(int(dilation.at<unsigned char>(y,x)) != 255) continue;
			// If outside the admissible region (e.g. in the zero region) , always INF
			if(seedimg.at<int>(y,x) == 0) continue;
			// If in another region's seed, or the new seed, always INF
			for(int j=0;j<offlimits.size();j++)
				if(x==offlimits.at(j).x && y==offlimits.at(j).y)
					zero=false;
			if(zero)
				data[(x+y*seedimg.size().width)*num_labels+l] = 0;
		}
	}

	Mat edged = edge.clone();
	//dilate(edge,edged,getStructuringElement(MORPH_RECT,Size(dilate_amount,dilate_amount)),Point(-1,-1),1,BORDER_CONSTANT,Scalar(0));
	dilate(edge,edged,getStructuringElement(MORPH_RECT,Size(dilate_amount,dilate_amount)));

	FORxyM(seedimg) {
		// Always set seed, regardless of whether it is within the dilation or not
		//if(x==seed.x && y==seed.y)
			//data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = 0; 
		//else
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = INF; 
		
		vector<Point> offlimits;
		for(int i=0;i<centroids.size();i++)
			offlimits.push_back(centroids.at(i));

		bool zero=true;
		if(int(edged.at<unsigned char>(y,x)) != 255) continue;
		if(seedimg.at<int>(y,x) == 0) continue;
		for(int i=0;i<offlimits.size();i++)
			if(x==offlimits.at(i).x && y==offlimits.at(i).y)
				zero=false;
		if(zero)
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = 0;
	}
	// Always set seed, regardless of whether it is within the dilation or not
	for(int i=0;i<seeds.size();i++)
		if(boundsCheck(seedimg,seeds.at(i)))
			data[(seeds.at(i).x+seeds.at(i).y*seedimg.size().width)*num_labels+num_labels-1] = 0;
	
	return data;
}	/*}}}*/
int * deleteDataTerm(Mat seedimg,vector<int> regions,int dilate_amount) {/*{{{*/
	int num_labels = regions.size()+1;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int *data = new int[num_pixels*num_labels];

	// Create zero region first
	FORxyM(seedimg)
		if(seedimg.at<int>(y,x) == 0)
			data[(x+y*seedimg.size().width)*num_labels+0] = 0;
		else
			data[(x+y*seedimg.size().width)*num_labels+0] = INF;

	for(int l=1;l<num_labels;l++) {
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		if(dilate_amount > 0)
			dilate(layer,dilation,getStructuringElement(MORPH_ELLIPSE,Size(dilate_amount,dilate_amount)));
		FORxyM(seedimg)
			data[(x+y*seedimg.size().width)*num_labels+l] = (int(dilation.at<unsigned char>(y,x)) == 255 && seedimg.at<int>(y,x) != 0 ? 0 : INF);
	}
	
	return data;
}	/*}}}*/
int * graphCut(int* data, int* sites, Mat seedimg, Mat adj,int num_labels, bool initialize) {/*{{{*/

	//int num_labels = mat_max(seedimg)+1;
	int num_pixels = seedimg.size().width*seedimg.size().height;

	int *result = new int[num_pixels];

	try {
		//cout << "@grid graph" << endl;
	
		// Setup grid-based graph
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(seedimg.size().width,seedimg.size().height,num_labels);

		// Initialize labels
		if(initialize)
			for(int x=0;x<seedimg.size().width;x++) for(int y=0;y<seedimg.size().height;y++)
				gc->setLabel(sub2ind(x,y,seedimg.size().width),seedimg.at<int>(y,x));

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

		//cout << "@alpha-beta expansion" << endl;
	
		//cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

		gc->swap(1);
		//gc->expansion(1);

		// Retrieve labeling
		for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);

		//cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;


		if(gc->giveDataEnergy() > 0)
			cout << "WARNING: NONZERO DATA COST" << endl;

		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

	return result;
}/*}}}*/
Mat globalGraphCut(Mat img, Mat seedimg,int dilate_amount) {/*{{{*/
	int width = img.size().width; 
	int height = img.size().height;
	int num_pixels = width*height;

	cout << "-image size: \t" << width  << "x" << height << endl;

	int num_labels = mat_max(seedimg)+1;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }
	cout << "-labels: \t" <<  num_labels << endl;

	cout << "preprocessing" << endl;

	cout << "@adjacency" << endl;
	Mat adj = regionsAdj(seedimg,num_labels);

	int *data = globalDataTerm(seedimg,dilate_amount);

	cout << "@sites" << endl;
	int *sites = toLinear(img);

	cout << "@graphcut" << endl;
	int *result = graphCut(data,sites,seedimg,adj,mat_max(seedimg)+1);
	Mat new_seed = toMat(result,width,height);

	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;

}/*}}}*/
Mat deleteGraphCut(Mat img, Mat seedimgin, vector<int> regions, int delreg) {/*{{{*/
	int num_labels = regions.size()+1;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }
	Mat seedimg = seedimgin.clone();
	Mat adj(num_labels,num_labels,CV_32S,1); // Everything adjacent

	int *data = deleteDataTerm(seedimg,regions,20);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj,num_labels);

	Mat new_seed = toMat(result,img.size().width,img.size().height);

	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;
}/*}}}*/
Mat junctionGraphCut(Mat img, Mat seedimgin, Point center, vector<int> regions, Point seed) {/*{{{*/
	//cout << "@subregion \t" << img.size().width << "," << img.size().height << endl;
	int num_labels = regions.size()+2;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }
	
	Mat seedimg = seedimgin.clone();

	// All adjacent
	Mat adj(num_labels,num_labels,CV_32S,1);

	if(boundsCheck(seedimg,seed))
		seedimg.at<int>(seed)=regions.size()+1;
	
	int *data = junctionDataTerm(seedimg,center,regions,seed,20);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj,num_labels);

	Mat new_seed = toMat(result,img.size().width,img.size().height);

	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;
}/*}}}*/
Mat edgeGraphCut(Mat img, Mat seedimgin, Mat edge, vector<int> regions, Point seed) {/*{{{*/
	//cout << "@subregion \t" << img.size().width << "," << img.size().height << endl;
	int num_labels = regions.size()+2;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }

	Mat seedimg = seedimgin.clone();

	Mat adj(num_labels,num_labels,CV_32S,1);

	if(boundsCheck(seedimg,seed))
		seedimg.at<int>(seed)=regions.size()+1;
	if(boundsCheck(seedimg,Point(seed.x+1,seed.y)))
		seedimg.at<int>(Point(seed.x+1,seed.y))=regions.size()+1;
	if(boundsCheck(seedimg,Point(seed.x,seed.y+1)))
		seedimg.at<int>(Point(seed.x,seed.y+1))=regions.size()+1;
	if(boundsCheck(seedimg,Point(seed.x+1,seed.y+1)))
		seedimg.at<int>(Point(seed.x+1,seed.y+1))=regions.size()+1;

	int *data = edgeDataTerm(seedimg,edge,regions,seed,40);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj,num_labels);

	Mat new_seed = toMat(result,img.size().width,img.size().height);

	Mat composite=overlay(new_seed,img,0.5);

	// DEBUG
	bool displayterm = false;
	FORxyM(new_seed) {
		if(data[(x+y*seedimg.size().width)*num_labels+new_seed.at<int>(y,x)] > 0){ 
			displayterm = true;
			cout<<"Offender:"<<x<<","<<y<<","<<new_seed.at<int>(y,x)<<":"<<data[(x+y*seedimg.size().width)*num_labels+new_seed.at<int>(y,x)]<<endl;
			cout<<"Seed    :"<<seed.x<<","<<seed.y<<endl;
			circle(composite,Point(x,y),3,Scalar(0,255,0,255));
		}
	}
	if(displayterm)
		display("tmp",composite);
	// /DEBUG

	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;
}/*}}}*/
vector< vector<int> > selectSeedPoints(Mat seedimg, Point center, int r) {/*{{{*/
	vector< vector<int> > candidates;
	// Generate rasterized poitns on circle (and shift to center)
	for(float theta=0;theta<2*PI;theta=theta+PI/24){
		vector<int> candidate;
		int x=r*cos(theta);
		int y=r*sin(theta);
		candidate.push_back(x+center.x);
		candidate.push_back(y+center.y);
		candidate.push_back(seedimg.at<int>(y+center.y,x+center.x));
		if(candidate[2] != -1 && candidate[0] >= 0 && candidate[1] >= 0 && candidate[0] < seedimg.size().width && candidate[1] < seedimg.size().height) // Exclude background candidates
			candidates.push_back(candidate);
	}

	// Get "derivative" of region changes
	vector<int> changept;
	for(int i=0;i<candidates.size()-1;i++)
		if(candidates[i][2] != candidates[i+1][2])
			changept.push_back(i);

	if(candidates[0][2] != candidates[candidates.size()-1][2])
		changept.push_back(0);

	// Find midpoints between derivatives
	vector<int> seedpt;
	for(int i=0;i<changept.size()-1;i++) {
		seedpt.push_back(int((changept[i+1]+changept[i])/2));
	}
	// Wrap around
	seedpt.push_back(((changept[changept.size()-1] + candidates.size() + changept[0])/2)%candidates.size());

	// Use midpoint indices to output full candidate points
	vector< vector<int> > output;
	for(int i=0;i<seedpt.size();i++)
		output.push_back(candidates[seedpt[i]]);

	return output;

}/*}}}*/
Mat shiftSubregion(Mat seedimg, vector<int> regions) {/*{{{*/
	// Shift all specified regions to continuous 1,2,... regions
	// 0 is reserved for all non-considered regions
	Mat shifted_seed(seedimg.size(),CV_32S,-1);
	FORxyM(seedimg) {
		int val=seedimg.at<int>(y,x);
		if(val==-1)
			if(boundsCheck(shifted_seed,Point(x,y)))
				shifted_seed.at<int>(y,x)=0;
		for(int i=0;i<regions.size();i++)
			if(val==regions[i])
				if(boundsCheck(shifted_seed,Point(x,y)))
					shifted_seed.at<int>(y,x)=i+1;
	}
	return shifted_seed;
}/*}}}*/
Mat shiftBackSubregion(Mat seedimg, Mat newseedimg, vector<int> regions) {/*{{{*/
	// :Shift all continuous regions (1,2,3,...) back to their respective
	//  actual, not-necessarily-continuous labels
	// :Anything in the 0 region is ignored 
	// :-2 is reserved for the newly-introduced region
	Mat new_shifted_seed(newseedimg.size(),CV_32S,-1);
	FORxyM(newseedimg) {
		int origval=seedimg.at<int>(y,x);
		int newval=newseedimg.at<int>(y,x);
		if(origval != -1) { // Only change stuff inside the boundary
			if(newval < regions.size()+1 && newval > 0) {
				if(boundsCheck(new_shifted_seed,Point(x,y)))
					new_shifted_seed.at<int>(y,x) = regions[newval-1];
			}
			else if(newval == 0) {
				if(boundsCheck(new_shifted_seed,Point(x,y)))
					new_shifted_seed.at<int>(y,x) = -1;
			}
			else {
				// Use -2 to represent new region
				if(boundsCheck(new_shifted_seed,Point(x,y)))
					new_shifted_seed.at<int>(y,x) = -2;
			}
		}
	}
	return new_shifted_seed;
}	/*}}}*/
Mat processJunctions(Mat img, Mat seedimg) {/*{{{*/
	cout << "@junctions \t\\" << flush;
	Mat seedout = seedimg.clone();
	int num_regions = mat_max(seedimg)+1;
	vector<int> sizes = regionSizes(seedimg);
	// Identify junctions
	vector< vector<int> > junctions = junctionRegions(seedimg);

	//Mat composite = overlay(seedimg,img);
	//for(int i=0;i<junctions.size();i++)
		//circle(composite,Point(junctions[i][0],junctions[i][1]),5,Scalar(255,255,255,255));
	//display("junctions",composite);

	int numbernew = 0; // Step through junctions, processing each
	for(int i=0;i<junctions.size();i++) {
		cout << "\b" << flush;
		cout << bar[i%4] << flush;
		//if(i!=159) continue;
		// Setup variables
		Point center(junctions[i][0],junctions[i][1]);
		vector<int> regions(3,-1);
		regions[0]=junctions[i][2];
		regions[1]=junctions[i][3];
		regions[2]=junctions[i][4];

		// Compute !win.contains(Point(x,y))window surrounding regions
		Rect win = getWindow(regions,seedimg);
		center.x = center.x - win.x;
		center.y = center.y - win.y;

		// Crop out subregion
		Mat imgj = img(win);	
		Mat seedj = seedimg(win).clone();

		// Zero out (-1 out, actually) background regions
		seedj = clearRegions(seedj,regions); 
		//clearRegions(seedj,regions); 

		Mat shifted_seed = shiftSubregion(seedj,regions);

		vector< vector<int> > seeds = selectSeedPoints(shifted_seed,center,4);
		vector< vector<int> > seeds2 = selectSeedPoints(shifted_seed,center,7);
		//vector< vector<int> > seeds3 = selectSeedPoints(shifted_seed,center,10);
		//vector< vector<int> > seeds4 = selectSeedPoints(shifted_seed,center,15);
		seeds.insert(seeds.end(),seeds2.begin(),seeds2.end());
		//seeds.insert(seeds.end(),seeds3.begin(),seeds3.end());
		//seeds.insert(seeds.end(),seeds4.begin(),seeds4.end());

		for(int j=0;j<seeds.size();j++) {
			// Compute graph cut on subregion
			Mat seedj_new = junctionGraphCut(imgj,shifted_seed,center,regions,Point(seeds[j][0],seeds[j][1]));
			Mat backshift_seed = shiftBackSubregion(seedj,seedj_new,regions);

			// Region criterion
			if(regionSize(backshift_seed,-2) < WINTHRESH) continue;

			if(regionSize(backshift_seed,regions[0]) < sizes[regions[0]]/3 ) continue;
			if(regionSize(backshift_seed,regions[1]) < sizes[regions[1]]/3 ) continue;
			if(regionSize(backshift_seed,regions[2]) < sizes[regions[2]]/3 ) continue;
			//if(!regionBorderCriteria(imgj,backshift_seed,-2,0.75)) continue;
			if(regionBorderCriteria(imgj,backshift_seed,-2,regions[0]) < 0.66) continue;
			if(regionBorderCriteria(imgj,backshift_seed,-2,regions[1]) < 0.66) continue;
			if(regionBorderCriteria(imgj,backshift_seed,-2,regions[3]) < 0.66) continue;

			//if(i==159) {
				//cout << i << endl;
				//cout << regions[0] << "," << regions[1] << "," << regions[2] << endl;
				//cout << regionSize(backshift_seed,regions[0]) << " < " << sizes[regions[0]]/3 << endl;
				//cout << regionSize(backshift_seed,regions[1]) << " < " << sizes[regions[1]]/3 << endl;
				//cout << regionSize(backshift_seed,regions[2]) << " < " << sizes[regions[2]]/3 << endl;
				//cout << "S:" << regionSize(backshift_seed,-2) << endl;
				//printstats(seedj);
				//printstats(backshift_seed);
				////display("reg1",overlay(seedimg(win),img(win)));
				//Mat composite = overlay(backshift_seed,img(win));
				//circle(composite,Point(seeds[j][0],seeds[j][1]),2,Scalar(255,255,255,255));
				//cout << regionCentroid(seedj,regions[0]).x << "," << regionCentroid(seedj,regions[0]).y << endl;
				//cout << regionCentroid(seedj,regions[1]).x << "," << regionCentroid(seedj,regions[1]).y << endl;
				//cout << regionCentroid(seedj,regions[2]).x << "," << regionCentroid(seedj,regions[2]).y << endl;
				//circle(composite,regionCentroid(seedj,regions[0]),2,Scalar(255,255,255,255));
				//circle(composite,regionCentroid(seedj,regions[1]),2,Scalar(255,255,255,255));
				//circle(composite,regionCentroid(seedj,regions[2]),2,Scalar(255,255,255,255));
				//display("reg2",composite);
			//}

			numbernew++;
			int new_label = num_regions++;
			//Map region back to whole label matrix
			FORxyM(backshift_seed) {
				if(backshift_seed.at<int>(y,x)==-1) continue; //Not bg
				if(backshift_seed.at<int>(y,x)==-2) {
					if(boundsCheck(seedout,Point(x+win.x,y+win.y)))
						seedout.at<int>(y+win.y,x+win.x) = new_label;
				}
				else {
					if(boundsCheck(seedout,Point(x+win.x,y+win.y)))
						seedout.at<int>(y+win.y,x+win.x) = backshift_seed.at<int>(y,x);
				}
			}
		}
	}
	cout << "\b" << flush;
	cout << numbernew << endl;

	return seedout;

}/*}}}*/
Mat processDelete(Mat img, Mat seedimg) {/*{{{*/
	cout << "@delete \t\\" << flush;
	Mat seedout = seedimg.clone();
	int num_regions = mat_max(seedimg)+1;
	Mat adj = regionsAdj(seedimg,num_regions);

	vector<int> regsizes = regionSizes(seedimg);

	for(int l=0;l<num_regions;l++) {
		cout << "\b" << flush;
		cout << bar[l%4] << flush;

		if(regsizes.at(l) > 200 || regsizes.at(l) < 1) continue;

		//cout << l << ": " << regsizes.at(l) << endl;

		vector<int> regions = getAdj(adj,l);
		regions.push_back(l);

		Rect win = getWindow(regions,seedimg);

		Mat subimg = img(win);
		Mat subseed = seedimg(win).clone();

		//if(regsizes.at(l) > 50 && regionBorderCriteria(subimg,subseed,l,0.50)) continue;
		if(regsizes.at(l) > 50 ) continue;

		//display(zpnum(l,1),overlay(subseed,subimg,0.5,l));

		subseed = clearRegions(subseed,regions);
		//clearRegions(subseed,regions);

		Mat subseed_s = shiftSubregion(subseed,regions);

		Mat subseed_new = deleteGraphCut(subimg,subseed_s,regions,l);

		subseed = shiftBackSubregion(subseed,subseed_new,regions);

		//display(zpnum(l,1),overlay(subseed,subimg,0.5,l));

		FORxyM(subseed) {
			if(subseed.at<int>(y,x)==-1) continue; //Not bg
			else {
				if(boundsCheck(seedout,Point(x+win.x,y+win.y)))
					seedout.at<int>(y+win.y,x+win.x) = subseed.at<int>(y,x);
			}
		}
	}

	cout << "\b" << flush;
	cout << "Done" << endl;

	return seedout;

}/*}}}*/
Mat processEdges(Mat img, Mat seedimg) {/*{{{*/
	// Region pairs calculated
	// By convention, the first of the two pairs will be the "primary" region
	// that will be dilated into. The following special numbers are also used
	// -1 -> top boundary
	// -2 -> bottom boundary
	// -3 -> left boundary
	// -4 -> right boundary
	int sizethresh = 300;
	int lengththresh = 4;
	int num_labels = mat_max(seedimg)+1;
	int num_regions = mat_max(seedimg)+1;
	int number_new = 0;
	Mat seedout = seedimg.clone();
	Mat adj = regionsAdj(seedimg,num_labels);
	vector<int> sizes = regionSizes(seedimg);
	vector< pair<int,int> > regionpair;

	time_t start,end;
	time(&start);

	//Add all normal pairs of regions
	for(int l1=0;l1<num_labels;l1++) for(int l2=0;l2<num_labels;l2++) {
		if(!adj.at<int>(l1,l2)) continue;
		if(sizes.at(l1) < sizethresh) continue;
		if(sizes.at(l2) < sizethresh) continue;
		if(regionEdge(seedimg,l1,l2).size() < lengththresh) continue;
		regionpair.push_back(make_pair(l1,l2));
	}

	set<int> boundaries;

	//Top
	for(int x=0;x<seedimg.size().width;x++)
		boundaries.insert(seedimg.at<int>(0,x));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-1).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-1));
	//cout<<boundaries.size() <<endl;
	boundaries.clear();
	
	//Bottom 
	for(int x=0;x<seedimg.size().width;x++)
		boundaries.insert(seedimg.at<int>(seedimg.size().height-1,x));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-2).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-2));
	//cout<<boundaries.size() <<endl;
	boundaries.clear();

	//Left
	for(int y=0;y<seedimg.size().height;y++)
		boundaries.insert(seedimg.at<int>(y,0));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-3).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-3));
	//cout<<boundaries.size() <<endl;
	boundaries.clear();

	//Right
	for(int y=0;y<seedimg.size().height;y++)
		boundaries.insert(seedimg.at<int>(y,seedimg.size().width-1));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-4).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-4));
	//cout<<boundaries.size() <<endl;
	boundaries.clear();

	time(&end);

	cout << endl << "# " << difftime(end,start) << endl;

	cout << "@edges \t\\" << flush;

	for(int i=0;i<regionpair.size();i++) {
		cout << "\b" << flush;
		cout << bar[i%4] << flush;
		pair<int,int> regionp = regionpair.at(i);
		vector<int> regions;
		if(!(regionp.first < 0)) regions.push_back(regionp.first);
		if(!(regionp.second < 0))regions.push_back(regionp.second);
		if(regions.empty()) {cout << "Empty region!" << endl; exit(0);}

		Rect win = getWindow(regions,seedimg);
		Mat subimg = img(win);
		Mat subseed = seedimg(win).clone();

		subseed = clearRegions(subseed,regions);
		
		//display("tmp",overlay(subseed,subimg));

		Mat subseed_s = shiftSubregion(subseed,regions);

		vector<Point> edge = regionEdge(seedimg,regionp.first,regionp.second);
		Point edg_centroid = vectorCentroid(edge); 
		edg_centroid.x -= win.x; 
		edg_centroid.y -= win.y; 
		Point reg_centroid = regionCentroid(subseed,regionp.first);

		vector<Point> seeds;
		for(int r=(regionp.second < 0 ? 1 : 2);r<euclideanDist(edg_centroid,reg_centroid)/3;r+=2)
			seeds.push_back(pointDirection(edg_centroid,reg_centroid,r));

		if(regionp.second < 0) {
			int x1=0,x2=seedimg.size().width,y1=0,y2=seedimg.size().height;
			for(int j=0;j<edge.size();j++) {
				y1=max(y1,edge.at(j).y-win.y);
				y2=min(y2,edge.at(j).y-win.y);
				x1=max(x1,edge.at(j).x-win.x);
				x2=min(x2,edge.at(j).x-win.x);
			}
			for(int r=(regionp.second < 0 ? 1 : 2);r<euclideanDist(edg_centroid,reg_centroid)/3;r+=2) {
				seeds.push_back(pointDirection(Point(x1,y1),reg_centroid,r));
				seeds.push_back(pointDirection(Point(x2,y2),reg_centroid,r));
			}
		}

		// Construct edge image
		Mat edgeimg(subseed.size(),CV_8U,Scalar(0)); 
		for (int j=0;j<edge.size();j++)
			// I've been bitten by this before. Bounds check.
			if(boundsCheck(edgeimg,Point(edge.at(j).x-win.x,edge.at(j).y-win.y)))
				edgeimg.at<unsigned char>(edge.at(j).y-win.y,edge.at(j).x-win.x)=255;

		for(int s=0;s<seeds.size();s++) {
			Point seed = seeds.at(s);
			
			//if(i==528) {
				//Mat composite=overlay(subseed,subimg);
				//circle(composite,edg_centroid,3,Scalar(0,255,0,255));
				//circle(composite,reg_centroid,3,Scalar(0,255,0,255));
				//circle(composite,seed,3,Scalar(0,255,0,255));
				//display("tmp",composite);
			//}

			Mat subseed_new = edgeGraphCut(subimg,subseed_s,edgeimg,regions,seed);
			Mat subseedt = shiftBackSubregion(subseed,subseed_new,regions);

			//display("tmp",overlay(subseedt,subimg));

			bool admissible=true;

			// Resulting size theshold
			if(regionSize(subseedt,-2) < 20) admissible=false;

			// Adjacent size threshold
			for(int j=0;j<regions.size();j++)
				if(regionSize(subseedt,regions.at(j)) < sizes[regions.at(j)]/3 ) admissible=false;
			// Intensity threshold
			for(int j=0;j<regions.size();j++)
				if(regionBorderCriteria(subimg,subseedt,-2,regions.at(j)) < 0.66) admissible=false;

			//if(i==429) {
				//if(admissible) {
					//display("tmp",overlay(subseed,subimg));
					//display("tmp",overlay(subseedt,subimg));
				//}
			//}

			if(admissible) {
				number_new++;
				int new_label = num_labels++;
				//Map region back to whole label matrix
				FORxyM(subseedt) {
					if(subseedt.at<int>(y,x)==-1) continue; //Not bg
					if(subseedt.at<int>(y,x)==-2) {
						if(boundsCheck(seedout,Point(x+win.x,y+win.y)))
							seedout.at<int>(y+win.y,x+win.x) = new_label;
					}
					else  {
						if(boundsCheck(seedout,Point(x+win.x,y+win.y)))
							seedout.at<int>(y+win.y,x+win.x) = subseedt.at<int>(y,x);
					}
				}
			}
		}

		//Mat composite=overlay(subseed,subimg);
		//vector<Point> edge = regionEdge(seedimg,regionp.first,regionp.second);
		//for (int j=0;j<edge.size();j++) {
			//edge.at(j).x-=win.x; 
			//edge.at(j).y-=win.y; 
			//circle(composite,edge.at(j),1,Scalar(0,0,0,255));
		//}
		//display("tmp",composite);

		// Handle "normal" cases first
		//if(regionp.second >= 0) {

		//}
	}

	cout << "\b" << flush;
	cout << number_new << endl;

	return seedout;

}/*}}}*/
int main(int argc, char **argv) {/*{{{*/
	// Read in cmd line args/*{{{*/
	int dilate_amount = 20;
	int framenum = 1;
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

	if (optind < argc) {
		while (optind < argc)
			framenum = atoi(argv[optind++]);
	}/*}}}*/

	// Sanity checks on input/*{{{*/
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
	cout << "-dilate: \t" << dilate_amount << endl;/*}}}*/

	// Read files/*{{{*/
	
	// Read previous label matrix for propagation 
	string seedfile=labelpath+prefix+zpnum(framenum-1,FNAMELEN)+postfix+"."+labeltype;
string imgfile=datapath+prefix+zpnum(framenum,FNAMELEN)+postfix+"."+imgtype;
	//string filemap=outputpath + "maps/image" + zpnum(framenum,4) + ".tif";

	Mat img = imread(imgfile,0);
	//Mat map = imread(filemap,0);

	//Mat imgblend = combine(img,map);
	double meanVal = 0;
	
	cv::mean(img).convertTo(&meanVal,1);
	cout << "-mean: \t" << meanVal << endl;

	meanthresh = int(meanVal)*2;

	Mat seedimg = loadMat(seedfile,img.size().width,img.size().height);

	/*}}}*/

	// Processing /*{{{*/

	seedimg = regionClean(seedimg);

	seedimg = globalGraphCut(img,seedimg,dilate_amount);
	seedimg = regionClean(seedimg);

	seedimg = processDelete(img,seedimg);	
	seedimg = regionClean(seedimg);

	seedimg = processJunctions(img,seedimg);
	seedimg = regionClean(seedimg);

	seedimg = processEdges(img,seedimg);
	seedimg = regionClean(seedimg);

/*}}}*/

	// Output/*{{{*/

	//Mat composite = overlay(seedimg,img,0.5);

	//cout << ">writing \t" << outputpath << "overlay/image" << zpnum(framenum,FNAMELEN) << ".png" << endl;

	//display("tmp",overlay(seedimg,img,0.5));
	
	writeMat(labelpath+prefix+zpnum(framenum,FNAMELEN)+postfix+"."+labeltype,seedimg);

	//imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+".png",composite);

	return 0;//*}}}*/
}
