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
Mat selectRegion(Mat seedimg, int region) {/*{{{*/
	Mat layer(seedimg.size(),CV_8U); // Must use CV_8U for dilation
	for(int y=0;y<seedimg.size().height;y++) for(int x=0;x<seedimg.size().width;x++) 
		layer.at<unsigned char>(y,x) = (seedimg.at<int>(y,x) == region ? 255 : 0);
	return layer;
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

		//display("tmp",dilation);

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
int * graphCut(int* data, int* sites, Mat seedimg, Mat adj) {/*{{{*/

	int num_labels = mat_max(seedimg)+1;
	int num_pixels = seedimg.size().width*seedimg.size().height;

	int *result = new int[num_pixels];

	try {
		cout << "@grid graph" << endl;
		
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

		// Initialize labeling to previous slice 
		for ( int  i = 0; i < num_pixels; i++ ) result[i] = gc->whatLabel(i);

		cout << "@alpha-beta expansion" << endl;
	
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
int graphCutEnergy(int* data, int* sites, Mat seedimg, Mat adj) {/*{{{*/

	int num_labels = mat_max(seedimg)+1;
	int num_pixels = seedimg.size().width*seedimg.size().height;
	int energy = 0;

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

		cout << "-T: " << gc->compute_energy() << ", D: " << gc->giveDataEnergy() << ", S: " << gc->giveSmoothEnergy() << endl;

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

	int *result = graphCut(data,sites,seedimg,adj);
	Mat new_seed = toMat(result,width,height);

	//writeRaw(outputpath+"labels/image"+zpnum(1,FNAMELEN)+".labels",result,num_pixels);

	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;

}/*}}}*/
Mat localGraphCut(Mat img_orig, Mat seedimg_orig, int ptx, int pty, int boxsize) {/*{{{*/

	int x0 = max(0,ptx-boxsize), x1 = min(img_orig.size().width,ptx+boxsize);
	int y0 = max(0,pty-boxsize), y1 = min(img_orig.size().height,pty+boxsize);

	cout << "@extracting \t" << ptx << "," << pty << endl;

	return localGraphCut(img_orig(Range(y0,y1),Range(x0,x1)),seedimg_orig(Range(y0,y1),Range(x0,x1)));
}/*}}}*/
Mat localGraphCut(Mat img, Mat seedimg) {/*{{{*/
	cout << "@subregion \t" << img.size().width << "," << img.size().height << endl;

	int width = img.size().width; 
	int height = img.size().height;
	int num_pixels = width*height;

	int num_labels = mat_max(seedimg)+1;
	if(num_labels < 2) { cout << "Must have > 1 label" << endl;	exit(1); }

	Mat adj = regionsAdj(seedimg,num_labels);
	
	int *data = globalDataTerm(seedimg,0);

	int *sites = toLinear(img);

	int *result = graphCut(data,sites,seedimg,adj);

	Mat new_seed = toMat(result,width,height);

	delete [] data;
	delete [] sites;
	delete [] result;

	return new_seed;
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

	Mat test = localGraphCut(img,seedimg,300,300,20);

	Mat new_seed = globalGraphCut(imgblend,seedimg,dilate_amount);

	// Output/*{{{*/
	writeMat(outputpath+"labels/image"+zpnum(framenum,FNAMELEN)+".labels",new_seed);

	/*Mat composite = overlay(new_seed,img,0.5);
	Mat composite2 = overlay(seedimg,img,0.5);

	cout << ">writing \t" << outputpath << "overlay/image" << zpnum(framenum,FNAMELEN) << ".png";

	imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+".png",composite);
	imwrite(outputpath+"overlay/image"+zpnum(framenum,FNAMELEN)+"old.png",composite2);*/

	return 0;//*}}}*/
}
