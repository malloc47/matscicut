#include "matscicut.h"
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
	FORxyM(regions)
		sizes.at(regions.at<int>(y,x)) = sizes.at(regions.at<int>(y,x))+1;
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
Mat selectRegion(Mat seedimg, int region) {/*{{{*/
	Mat layer(seedimg.size(),CV_8U); // Must use CV_8U for dilation
	for(int y=0;y<seedimg.size().height;y++) for(int x=0;x<seedimg.size().width;x++) 
		layer.at<unsigned char>(y,x) = (seedimg.at<int>(y,x) == region ? 255 : 0);
	return layer;
}/*}}}*/
Mat clearRegions(Mat seedimg, vector<int> regions) {/*{{{*/	
	FORxyM(seedimg) {
		bool inRegion = false;
		for(int z=0;z<regions.size();z++)
			if(seedimg.at<int>(y,x) == regions[z]) inRegion=true;
		if(!inRegion) seedimg.at<int>(y,x) = -1;
	}
	return seedimg;
}/*}}}*/
Mat changeRegion(Mat seedimg, int region1, int region2) {/*{{{*/
	FORxyM(seedimg) 
		if(seedimg.at<int>(y,x)==region1)
			seedimg.at<int>(y,x)=region2;
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
int * junctionDataTerm(Mat seedimg,Point center,vector<int> regions,Point seed) {/*{{{*/
	int num_labels = regions.size()+2;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int *data = new int[num_pixels*num_labels];

	for(int l=0;l<num_labels-1;l++) {
		Mat layer = selectRegion(seedimg,l);
		FORxyM(seedimg)
			data[(x+y*seedimg.size().width)*num_labels+l] = (int(layer.at<unsigned char>(y,x)) == 255 && !(x==seed.x && y==seed.y) ? 0 : INF);
	}

	Rect win(center.x-ADDWIN,center.y-ADDWIN,ADDWIN*2,ADDWIN*2);
	//cout << win.x << "," << win.y << "," << win.width << "," << win.height << endl;

	FORxyM(seedimg) {
		data[(x+y*seedimg.size().width)*num_labels+num_labels-1] = ( ( win.contains(Point(x,y)) && seedimg.at<int>(y,x) > 0 ) ? 0 : INF);
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
	
		cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

		gc->swap(1);
		//gc->expansion(1);

		// Retrieve labeling
		for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);

		cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;


		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

	return result;
}/*}}}*/
int graphCutEnergy(int* data, int* sites, Mat seedimg, Mat adj, int num_labels) {/*{{{*/ 
	int num_pixels = seedimg.size().width*seedimg.size().height; int energy = 0;

	try {
		
		// Setup grid-based graph
		GCoptimizationGridGraph *gc = new GCoptimizationGridGraph(seedimg.size().width,seedimg.size().height,num_labels);

		// Initialize labels
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

		//cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

		energy = gc->giveSmoothEnergy(); 

		delete gc;
	}
	catch (GCException e) {
		e.Report();
	}

	return energy;
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
Mat junctionGraphCut(Mat img, Mat seedimgin, Point center, vector<int> regions, Point seed) {/*{{{*/
	cout << "@subregion \t" << img.size().width << "," << img.size().height << endl;
	int num_labels = regions.size()+2;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }
	
	Mat seedimg = seedimgin.clone();

	// All adjacent
	Mat adj(num_labels,num_labels,CV_32S,1);

	/*// I use -1 as "background" pixels
	Mat shifted_seed(seedimg.size(),CV_32S,-1);

	// Convert 
	FORxyM(seedimg) {
		int val=seedimg.at<int>(y,x);
		if(val==-1) shifted_seed.at<int>(y,x)=0;
		for(int i=0;i<regions.size();i++)
			if(val==regions[i])
				shifted_seed.at<int>(y,x)=i+1;
	}*/

	// Initialization with at least one label in center
	/*Rect seedreg(center.x-2,center.y-2,4,4);
	FORxyM(shifted_seed)
		if(seedreg.contains(Point(x,y)))
			shifted_seed.at<int>(y,x) = regions.size()+1;*/

	seedimg.at<int>(seed)=regions.size()+1;
	
	int *data = junctionDataTerm(seedimg,center,regions,seed);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj,num_labels);

	Mat new_seed = toMat(result,img.size().width,img.size().height);

	vector<int> sizes = regionSizes(new_seed);

	/*if(mat_max(new_seed) > 3 && sizes[4] > 30) {
		display("test",overlay(shifted_seed,img));
		display("test",overlay(new_seed,img));
	}*/

	/*Mat new_shifted_seed(seedimg.size(),CV_32S,-1);

	FORxyM(new_seed) {
		int origval=seedimg.at<int>(y,x);
		int newval=new_seed.at<int>(y,x);
		if(origval != -1) { // Only change stuff inside the boundary
			if(newval < regions.size()+1 && newval > 0)
				new_shifted_seed.at<int>(y,x) = regions[newval-1];
			else
				// Use -2 to represent new region
				new_shifted_seed.at<int>(y,x) = -2;
		}
	}*/
	
	delete [] data;
	delete [] sites;
	delete [] result;

	//return new_shifted_seed;
	return new_seed;
}/*}}}*/
vector< vector<int> > selectSeedPoints(Mat seedimg, Point center, int r) {/*{{{*/
	vector< vector<int> > candidates;
	// Generate rasterized poitns on circle (and shift to center)
	for(float theta=0;theta<2*PI;theta=theta+PI/12){
		vector<int> candidate;
		int x=r*cos(theta);
		int y=r*sin(theta);
		candidate.push_back(x+center.x);
		candidate.push_back(y+center.y);
		candidate.push_back(seedimg.at<int>(y+center.y,x+center.x));
		if(candidate[2] != -1) // Exclude background candidates
			candidates.push_back(candidate);
		//cout << x+center.x << "," << y+center.y << endl;
	}

	// Get "derivative" of region changes
	vector<int> changept;
	//cout << "Finding region changes" << endl;
	for(int i=0;i<candidates.size()-1;i++)
		if(candidates[i][2] != candidates[i+1][2])
			changept.push_back(i);

	if(candidates[0][2] != candidates[candidates.size()-1][2])
		changept.push_back(0);

	printVector(changept);

	// Find midpoints between derivatives
	//cout << "Finding change centers" << endl;
	vector<int> seedpt;
	for(int i=0;i<changept.size()-1;i++) {
		seedpt.push_back(int((changept[i+1]+changept[i])/2));
	}
	// Wrap around
	seedpt.push_back(((changept[changept.size()-1] + candidates.size() + changept[0])/2)%candidates.size());

	printVector(seedpt);

	// Use midpoint indices to output full candidate points
	vector< vector<int> > output;
	for(int i=0;i<seedpt.size();i++)
		output.push_back(candidates[seedpt[i]]);

	return output;

}/*}}}*/
Mat shiftSubregion(Mat seedimg, vector<int> regions) {/*{{{*/
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
	Mat new_shifted_seed(newseedimg.size(),CV_32S,-1);
	FORxyM(newseedimg) {
		int origval=seedimg.at<int>(y,x);
		int newval=newseedimg.at<int>(y,x);
		if(origval != -1) { // Only change stuff inside the boundary
			if(newval < regions.size()+1 && newval > 0)
				new_shifted_seed.at<int>(y,x) = regions[newval-1];
			else
				// Use -2 to represent new region
				new_shifted_seed.at<int>(y,x) = -2;
		}
	}
	return new_shifted_seed;
}	/*}}}*/
Mat processJunctions(Mat img, Mat seedimg) {/*{{{*/
	cout << "@localgcj" << endl;
	Mat seedout = seedimg.clone();
	int num_regions = mat_max(seedimg)+1;
	// Identify junctions
	vector< vector<int> > junctions = junctionRegions(seedimg);
	int numbernew = 0; // Step through junctions, processing each
	for(int i=0;i<junctions.size();i++) {
		// Setup variables
		Point center(junctions[i][0],junctions[i][1]);
		vector<int> regions(3,-1);
		regions[0]=junctions[i][2];
		regions[1]=junctions[i][3];
		regions[2]=junctions[i][4];

		// Compute window surrounding regions
		Rect win = getWindow(regions,seedimg);
		center.x = center.x - win.x;
		center.y = center.y - win.y;

		// Crop out subregion
		Mat imgj = img(win);	
		Mat seedj = seedimg(win).clone();

		// Zero out (-1 out, actually) background regions
		seedj = clearRegions(seedj,regions); 

		/*// I use -1 as "background" pixels
		Mat shifted_seed(seedj.size(),CV_32S,-1);

		// Convert 
		FORxyM(seedj) {
			int val=seedj.at<int>(y,x);
			if(val==-1) shifted_seed.at<int>(y,x)=0;
			for(int i=0;i<regions.size();i++)
				if(val==regions[i])
					shifted_seed.at<int>(y,x)=i+1;
		}*/

		Mat shifted_seed = shiftSubregion(seedj,regions);

		vector< vector<int> > seeds = selectSeedPoints(shifted_seed,center,5);

		for(int i=0;i<seeds.size();i++) {

			// Compute graph cut on subregion
			Mat seedj_new = junctionGraphCut(imgj,shifted_seed,center,regions,Point(seeds[i][0],seeds[i][1]));

			/*Mat new_shifted_seed(seedj.size(),CV_32S,-1);

			FORxyM(seedj_new) {
				int origval=seedj.at<int>(y,x);
				int newval=seedj_new.at<int>(y,x);
				if(origval != -1) { // Only change stuff inside the boundary
					if(newval < regions.size()+1 && newval > 0)
						new_shifted_seed.at<int>(y,x) = regions[newval-1];
					else
						// Use -2 to represent new region
						new_shifted_seed.at<int>(y,x) = -2;
				}
			}*/

			Mat backshift_seed = shiftBackSubregion(seedj,seedj_new,regions);

			display("reg1",overlay(backshift_seed,img(win)));


		}
		
		//Subregion criterion
		/*if(regionSize(seedj_new,-2) > WINTHRESH) {
			cout << regions[0] << "," << regions[1] << "," << regions[2] << endl;
			cout << "S:" << regionSize(seedj_new,-2) << endl;
			printstats(seedj);
			printstats(seedj_new);
			display("reg1",overlay(seedimg(win),img(win)));
			display("reg2",overlay(seedj_new,img(win)));

			Compute new label
			numbernew++;
			int new_label = num_regions++;
			Map region back to whole label matrix
			FORxyM(seedj_new) {
				if(seedj_new.at<int>(y,x)==-1) continue; Not bg
				if(seedj_new.at<int>(y,x)==-2)
					seedout.at<int>(y+win.y,x+win.x) = new_label;
				else
					seedout.at<int>(y+win.y,x+win.x) = seedj_new.at<int>(y,x);
			}
		}*/
	}

	//display("final",overlay(seedout,img));
	
	cout << "New: " << numbernew << endl;

	return seedout;

}/*}}}*/
Mat processSmall(Mat img, Mat seedimg) {/*{{{*/

	Mat adj = regionsAdj(seedimg,mat_max(seedimg)+1);

	vector<int> regsizes = regionSizes(seedimg);

	for(int l=0;l<mat_max(seedimg)+1;l++) {
		
		if(regsizes.at(l) > 1000 || regsizes.at(l) < 1) continue;

		cout << l << ": " << regsizes.at(l) << endl;

		vector<int> labels = getAdj(adj,l);

		Rect test = getWindow(labels,seedimg);

		cout << test.x << "," << test.y << "," << test.width << "," << test.height << endl;

		Mat imgcuttest = img(test);
		Mat imgseedtest = seedimg(test);

		//display(zpnum(l,1),overlay(imgseedtest,imgcuttest,0.5,l));
	}

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

	Mat tmp_seed = globalGraphCut(imgblend,seedimg,dilate_amount);
	Mat new_seed= processJunctions(img,tmp_seed);


/*}}}*/

	// Output/*{{{*/
	writeMat(outputpath+"labels/image"+zpnum(framenum,FNAMELEN)+".labels",new_seed);

	Mat composite = overlay(new_seed,img,0.5);
	  Mat composite2 = overlay(seedimg,img,0.5);

	  cout << ">writing \t" << outputpath << "overlay/image" << zpnum(framenum,FNAMELEN) << ".png";

	  display("tmp",composite2);
	  display("tmp",composite);

	  //imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+".png",composite);
	  //imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+"old.png",composite2);*/

	return 0;//*}}}*/
}
