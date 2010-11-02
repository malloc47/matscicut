#include "matscicut.h"
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
		adj.at<int>(i,j)=0;

	// Horizontal, including first, exluding last column
	for(int x=0;x<regions.size().width-1;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x+1);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
			}
		}
	}

	for(int x=0;x<regions.size().width-2;x++) {
		for(int y=0;y<regions.size().height;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y,x+2);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
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
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
			}
		}
	}
	
	for(int x=0;x<regions.size().width;x++) {
		for(int y=0;y<regions.size().height-2;y++) {
			int r1 = regions.at<int>(y,x);
			int r2 = regions.at<int>(y+2,x);
			if( r1 != r2 ) {
				adj.at<int>(r1,r2) = 1;
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
				adj.at<int>(r1,r2) = 1;
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
				adj.at<int>(r1,r2) = 1;
				adj.at<int>(r2,r1) = 1;
			}
		}
	}

	return adj;
}/*}}}*/
int regionNum(Mat regions) {/*{{{*/
	int count=0;
	int max_regions=mat_max(regions);

	vector<int> regioncount(mat_max(regions)+1,0);

	FORxyM(regions) {
		regioncount.at(regions.at<int>(y,x))++;
	}

	for(int i=0;i<regioncount.size();i++)
		if(regioncount.at(i) > 0)
			count++;
	
	return count;

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
						regions.at<int>(y,x)--; // Decrement by one
						clean=false;
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
				regions_out.at<int>(y,x) = l;
		
	}


	while(mat_min(regions_out) < 0) {
		cout << "Cleaning" << endl;
		FORxyM(regions_out) {
			if(regions_out.at<int>(y,x) < 0) {
				if(x!=0 && regions_out.at<int>(y,x-1) != -1) {
					regions_out.at<int>(y,x) = regions_out.at<int>(y,x-1);
					continue;
				}
				if(y!=0 && regions_out.at<int>(y-1,x) != -1) {
					regions_out.at<int>(y,x) = regions_out.at<int>(y-1,x);
					continue;
				}
				if(x!=regions_out.size().width-1 && regions_out.at<int>(y,x+1) != -1) {
					regions_out.at<int>(y,x) = regions_out.at<int>(y,x+1);
					continue;
				}
				if(y!=regions_out.size().height-1 && regions_out.at<int>(y+1,x) != -1) {
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
bool regionBorderCriteria(Mat img, Mat regions, int region, float thresh) {/*{{{*/
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

	cout << (total_thresh/total_border) << " > " << thresh << endl;
	


	return (total_thresh/total_border> thresh);
	
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

	return Rect(x0,y0,x1-x0,y1-y0);

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
		layer.at<unsigned char>(y,x) = (seedimg.at<int>(y,x) == region ? 255 : 0);
	return layer;
}/*}}}*/
void clearRegions(Mat seedimg, vector<int> regions) {/*{{{*/	
	FORxyM(seedimg) {
		bool inRegion = false;
		for(int z=0;z<regions.size();z++)
			if(seedimg.at<int>(y,x) == regions[z]) inRegion=true;
		if(!inRegion) seedimg.at<int>(y,x) = -1;
	}
	//return seedimg;
}/*}}}*/
int * globalDataTerm(Mat seedimg,int dilate_amount) {/*{{{*/

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

	//for(int i=0;i<centroids.size();i++) {
		//cout << regions.at(i)  << ":" << centroids.at(i).x << "," << centroids.at(i).y << endl;
	//}

	for(int l=0;l<num_labels-1;l++) {
		//Point centroid = regionCentroid(seedimg,l);
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		if(dilate_amount > 0 && l!=0)
			dilate(layer,dilation,getStructuringElement(MORPH_ELLIPSE,Size(dilate_amount,dilate_amount)));
		FORxyM(seedimg) {
			// Set to inf by default
			data[(x+y*seedimg.size().width)*num_labels+l] = INF;
			bool zero=true;
			if(int(dilation.at<unsigned char>(y,x)) != 255) continue;
			if(seedimg.at<int>(y,x) == 0) continue;
			if((x==seed.x && y==seed.y)) continue; 
			for(int i=0;i<centroids.size();i++)
				if((i+1 != l) && (x==centroids.at(i).x && y==centroids.at(i).y))
					zero=false;

			if(zero)
				data[(x+y*seedimg.size().width)*num_labels+l] = 0;
		}
	}

	Rect win(center.x-ADDWIN,center.y-ADDWIN,ADDWIN*2,ADDWIN*2);

	FORxyM(seedimg) {
		data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = INF; 
		bool zero=true;
		if(!win.contains(Point(x,y))) continue;
		if(!(seedimg.at<int>(y,x) > 0)) continue;
			for(int i=0;i<centroids.size();i++)
				if((x==centroids.at(i).x && y==centroids.at(i).y))
					zero=false;
		if(zero)
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = 0;
	}
	
	return data;
}	/*}}}*/
int * edgeDataTerm(Mat seedimg,Mat edge,vector<int> regions,Point seed,int dilate_amount) {/*{{{*/
	int num_labels = regions.size()+2;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int *data = new int[num_pixels*num_labels];

	vector<Point> centroids(regions.size());
	for(int i=0;i<regions.size();i++)
		centroids.at(i) = regionCentroid(seedimg,i+1);

	for(int l=0;l<num_labels-1;l++) {
		//Point centroid = regionCentroid(seedimg,l);
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		if(dilate_amount > 0 && l!=0)
			dilate(layer,dilation,getStructuringElement(MORPH_ELLIPSE,Size(dilate_amount,dilate_amount)));
		FORxyM(seedimg) {
			// Set to inf by default
			data[(x+y*seedimg.size().width)*num_labels+l] = INF;
			bool zero=true;
			if(int(dilation.at<unsigned char>(y,x)) != 255) continue;
			if(seedimg.at<int>(y,x) == 0) continue;
			if((x==seed.x && y==seed.y)) continue; 
			for(int i=0;i<centroids.size();i++)
				if((i+1 != l) && (x==centroids.at(i).x && y==centroids.at(i).y))
					zero=false;
			if(zero)
				data[(x+y*seedimg.size().width)*num_labels+l] = 0;
		}
	}

	Mat edged = edge.clone();
	dilate(edge,edged,getStructuringElement(MORPH_ELLIPSE,Size(dilate_amount,dilate_amount)));
	//display("tmp",edged);
	FORxyM(seedimg) {
		data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = INF; 
		bool zero=true;
		if(!(edged.at<unsigned char>(y,x) > 0)) continue;
		if(!(seedimg.at<int>(y,x) > 0)) continue;
			for(int i=0;i<centroids.size();i++)
				if((x==centroids.at(i).x && y==centroids.at(i).y))
					zero=false;
		if(zero)
			data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = 0;
	}
	
	return data;
}	/*}}}*/
int * deleteDataTerm(Mat seedimg,vector<int> regions,int dilate_amount) {/*{{{*/
	int num_labels = regions.size()+1;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int *data = new int[num_pixels*num_labels];

	for(int l=0;l<num_labels;l++) {
		Mat layer = selectRegion(seedimg,l);
		Mat dilation = layer.clone();
		if(dilate_amount > 0 && l!=0)
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

	seedimg.at<int>(seed)=regions.size()+1;
	
	int *data = junctionDataTerm(seedimg,center,regions,seed,20);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj,num_labels);

	Mat new_seed = toMat(result,img.size().width,img.size().height);

	//vector<int> sizes = regionSizes(new_seed);
	
	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;
}/*}}}*/
Mat edgeGraphCut(Mat img, Mat seedimgin, Mat edge, vector<int> regions, Point seed) {/*{{{*/
	//cout << "@subregion \t" << img.size().width << "," << img.size().height << endl;
	int num_labels = regions.size()+2;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }

	cout << "Loop" << endl;

	//FORxyM(seedimgin) cout << seedimgin.at<int>(y,x) << " " << endl;
	
	cout << "Clone" << endl;
	
	//Mat seedimg = seedimgin.clone();
	Mat seedimg(seedimgin.size(),CV_32S);
	FORxyM(seedimgin) seedimg.at<int>(y,x) = seedimgin.at<int>(y,x);

	cout << "Adj" << endl;

	Mat adj(num_labels,num_labels,CV_32S,1);
	
	seedimg.at<int>(seed)=regions.size()+1;

	int *data = edgeDataTerm(seedimg,edge,regions,seed,20);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj,num_labels);

	Mat new_seed = toMat(result,img.size().width,img.size().height);

	//vector<int> sizes = regionSizes(new_seed);
	
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
		if(val==-1) shifted_seed.at<int>(y,x)=0;
		for(int i=0;i<regions.size();i++)
			if(val==regions[i])
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
			if(newval < regions.size()+1 && newval > 0)
				new_shifted_seed.at<int>(y,x) = regions[newval-1];
			else if(newval == 0)
				new_shifted_seed.at<int>(y,x) = -1;
			else {
				// Use -2 to represent new region
				new_shifted_seed.at<int>(y,x) = -2;
			}
		}
	}
	return new_shifted_seed;
}	/*}}}*/
Mat processJunctions(Mat img, Mat seedimg) {/*{{{*/
	cout << "@localgcj" << endl;
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
		//seedj = clearRegions(seedj,regions); 
		clearRegions(seedj,regions); 

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
			if(!regionBorderCriteria(imgj,backshift_seed,-2,0.66)) continue;

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
				if(backshift_seed.at<int>(y,x)==-2)
					seedout.at<int>(y+win.y,x+win.x) = new_label;
				else
					seedout.at<int>(y+win.y,x+win.x) = backshift_seed.at<int>(y,x);
			}
		}
	}

	cout << "New: " << numbernew << endl;

	return seedout;

}/*}}}*/
Mat processDelete(Mat img, Mat seedimg) {/*{{{*/
	cout << "@localgcs" << endl;
	Mat seedout = seedimg.clone();
	int num_regions = mat_max(seedimg)+1;
	Mat adj = regionsAdj(seedimg,num_regions);

	vector<int> regsizes = regionSizes(seedimg);

	for(int l=0;l<num_regions;l++) {
		
		if(regsizes.at(l) > 200 || regsizes.at(l) < 1) continue;

		cout << l << ": " << regsizes.at(l) << endl;

		vector<int> regions = getAdj(adj,l);
		regions.push_back(l);

		Rect win = getWindow(regions,seedimg);

		Mat subimg = img(win);
		Mat subseed = seedimg(win).clone();

		if(regsizes.at(l) > 50 && regionBorderCriteria(subimg,subseed,l,0.50)) continue;

		//display(zpnum(l,1),overlay(subseed,subimg,0.5,l));

		//subseed = clearRegions(subseed,regions);
		clearRegions(subseed,regions);

		Mat subseed_s = shiftSubregion(subseed,regions);

		Mat subseed_new = deleteGraphCut(subimg,subseed_s,regions,l);

		subseed = shiftBackSubregion(subseed,subseed_new,regions);

		//display(zpnum(l,1),overlay(subseed,subimg,0.5,l));

		FORxyM(subseed) {
			if(subseed.at<int>(y,x)==-1) continue; //Not bg
			else seedout.at<int>(y+win.y,x+win.x) = subseed.at<int>(y,x);
		}
	}

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
	int sizethresh = 500;
	int lengththresh = 50;
	int num_labels = mat_max(seedimg)+1;
	Mat seedout = seedimg.clone();
	Mat adj = regionsAdj(seedimg,num_labels);
	vector<int> sizes = regionSizes(seedimg);
	vector< pair<int,int> > regionpair;
	// Add all normal pairs of regions
	for(int l1=0;l1<num_labels;l1++) for(int l2=0;l2<num_labels;l2++) {
		if(!adj.at<int>(l1,l2)) continue;
		if(sizes.at(l1) < sizethresh) continue;
		//if(sizes.at(l2) < sizethresh) continue;
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
	boundaries.clear();
	
	//Bottom 
	for(int x=0;x<seedimg.size().width;x++)
		boundaries.insert(seedimg.at<int>(seedimg.size().height-1,x));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-2).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-2));
	boundaries.clear();

	//Left
	for(int y=0;y<seedimg.size().height;y++)
		boundaries.insert(seedimg.at<int>(y,0));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-3).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-3));
	boundaries.clear();

	//Right
	for(int y=0;y<seedimg.size().height;y++)
		boundaries.insert(seedimg.at<int>(y,seedimg.size().width-1));
	for (set<int>::iterator it=boundaries.begin() ; it != boundaries.end(); it++ )
		if(sizes.at(*it) >= sizethresh && regionEdge(seedimg,*it,-4).size() > lengththresh)
			regionpair.push_back(make_pair(*it,-4));
	boundaries.clear();

	cout << "@localgce \t" << regionpair.size() << endl;

	for(int i=0;i<regionpair.size();i++) {
		if(i<230) continue;
		cout << i << " of " << regionpair.size() << endl;
		pair<int,int> regionp = regionpair.at(i);
		vector<int> regions;
		if(!(regionp.first < 0)) regions.push_back(regionp.first);
		if(!(regionp.second < 0))regions.push_back(regionp.second);
		if(regions.empty()) {cout << "Empty region!" << endl; exit(0);}

		Rect win = getWindow(regions,seedimg);
		Mat subimg = img(win);
		Mat subseed = seedimg(win).clone();

		clearRegions(subseed,regions);
		Mat subseed_s = shiftSubregion(subseed,regions);

		vector<Point> edge = regionEdge(seedimg,regionp.first,regionp.second);
		Point edg_centroid = vectorCentroid(edge); 
		edg_centroid.x -= win.x; 
		edg_centroid.y -= win.y; 
		Point reg_centroid = regionCentroid(subseed,regionp.first);

		int r=7;
		int xbearing = reg_centroid.x - edg_centroid.x;
		int ybearing = reg_centroid.y - edg_centroid.y;
		float theta = atan2(ybearing,xbearing);
		Point seed(r*cos(theta)+edg_centroid.x,r*sin(theta)+edg_centroid.y);
		
		Mat composite=overlay(subseed,subimg);
		circle(composite,edg_centroid,3,Scalar(0,255,0,255));
		circle(composite,reg_centroid,3,Scalar(0,255,0,255));
		circle(composite,seed,3,Scalar(0,255,0,255));
		//display("tmp",composite);

		// Construct edge image
		Mat edgeimg(subseed.size(),CV_8U); 
		FORxyM(edgeimg) edgeimg.at<unsigned char>(y,x) = 0;
		for (int j=0;j<edge.size();j++)
			edgeimg.at<unsigned char>(edge.at(j).y-win.y,edge.at(j).x-win.x)=255;
		//display("tmp",edgeimg);
		
		Mat subseed_new = edgeGraphCut(subimg,subseed_s,edgeimg,regions,seed);

		subseed = shiftBackSubregion(subseed,subseed_new,regions);

		//display("tmp",overlay(subseed,subimg));

		//FORxyM(subseed) {
			//if(subseed.at<int>(y,x)==-1) continue; //Not bg
			//else seedout.at<int>(y+win.y,x+win.x) = subseed.at<int>(y,x);
		//}

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
	string filea1=datapath + "4000_Series/4000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";/*{{{*/
	string filea2=datapath + "5000_Series/5000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string filea3=datapath + "6000_Series/6000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string filea4=datapath + "7000_Series/7000_image" + zpnum(framenum-1,FNAMELEN) + ".tif";
	string fileb1=datapath + "4000_Series/4000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string fileb2=datapath + "5000_Series/5000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string fileb3=datapath + "6000_Series/6000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string fileb4=datapath + "7000_Series/7000_image" + zpnum(framenum,FNAMELEN) + ".tif";
	string seedfile=outputpath + "labels/image" + zpnum(framenum-1,FNAMELEN)+".labels";

	string filenew=datapath + "stfl" + zpnum(framenum,2) + "alss1.tif";
	string filemap=outputpath + "maps/image" + zpnum(framenum,4) + ".tif";/*}}}*/

	Mat img = imread(filenew,0);
	Mat map = imread(filemap,0);

	Mat imgblend = combine(img,map);

	//Mat img = imread(fileb1,0);

	Mat seedimg = loadMat(seedfile,img.size().width,img.size().height);

	/*}}}*/

	// Processing /*{{{*/
	
	//Mat new_seed = processJunctions(img,seedimg);


	//for(int l=0;l<mat_max(seedimg)+1;l++) {
		//cout << l << endl;
		//display("tmp",overlay(seedimg,img,0.5,l));
	//}
	
	
	// Clean first
	seedimg = regionClean(seedimg);

	// Process global
	//Mat seedimg1 = globalGraphCut(imgblend,seedimg,dilate_amount);
	//seedimg1 = regionClean(seedimg1);
	
	//writeMat("tmp.labels",seedimg1);
	Mat seedimg1 = loadMat("tmp.labels",img.size().width,img.size().height);

	// Delete unneeded grains 
	//Mat seedimg2 = processDelete(img,seedimg1);	
	//seedimg2 = regionClean(seedimg2);

	// Add in missed junctions
	//Mat seedimg3 = processJunctions(img,seedimg2);
	//seedimg3 = regionClean(seedimg3);

	// Add in grains created at edges 
	Mat seedimg4 = processEdges(img,seedimg1);
	seedimg4 = regionClean(seedimg4);

	//display("tmp",overlay(new_seed,img,0.5));


/*}}}*/

	// Output/*{{{*/

	Mat composite = overlay(seedimg4,img,0.5);
	Mat composite2 = overlay(seedimg,img,0.5);

	cout << ">writing \t" << outputpath << "overlay/image" << zpnum(framenum,FNAMELEN) << ".png" << endl;

	//display("tmp",overlay(seedimg,img,0.5));
	//display("tmp",overlay(new_seed,img,0.5));
	
	writeMat(outputpath+"labels/image"+zpnum(framenum,FNAMELEN)+".labels",seedimg4);

	//imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+".png",composite);
	//imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+"old.png",composite2);*/

	return 0;//*}}}*/
}
