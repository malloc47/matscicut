function ncuts_eval2(imgnums)

	%addpath('../../programs/normalized-cuts/ncut_multiscale_1_6/');
	init
% $$$         source

        seqpath = 'seq9';
        
for imgnum = imgnums

	series = 1;
% 	datapath = '~/src/projects/matsci/matscicut/data/old/scaled/';
% 	outputpath = '~/src/projects/matsci/matscicut/outputn2/';
	datapath = ['~/src/projects/matsci/matscicut/' seqpath '/img/scaled/'];
	outputpath = ['~/src/projects/matsci/matscicut/' seqpath '/nc/'];


% 	ground = logical(imread(['~/src/projects/matsci/matscicut/data/old/scaled/ground/' 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = logical(rgb2gray(imread(['~/src/projects/matsci/matscicut/' seqpath '/img/ground/' 'image' sprintf('%04d',imgnum) '.png'])));
	ground_label = bwlabel(ground,4);
	ground_num = max(max(ground_label))

% 	img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
img = imread([datapath 'image' sprintf('%04d',imgnum) '.png']);
	%img2 = imhmin(img,20);
	[labels,X,lambda,Xr,W,C,timing] = ncut_multiscale(img,ground_num);
	%labels = watershed(img2);
	%labels = dlmread([outputpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	dlmwrite([outputpath 'labels/image' sprintf('%04d',imgnum) '.labels'],labels,' ');
	Lrgb = label2rgb(labels,'jet','w','shuffle');
	imwrite(Lrgb,[outputpath 'region/image' sprintf('%04d',imgnum) '.png'],'png');
	
	groundbmp = seg2bmap(labels);
	groundbmp = bwmorph(groundbmp,'thin',Inf);
	groundbmp = imdilate(groundbmp,strel('disk',2));

	output(:,:,1) = img;
	output(:,:,2) = img;
	output(:,:,3) = img;

	outputr = output(:,:,1);
	outputg = output(:,:,2);
	outputb = output(:,:,3);

	outputr(groundbmp) = 255;
	outputg(groundbmp) = 0;
	outputb(groundbmp) = 0;

	output(:,:,1) = outputr;
	output(:,:,2) = outputg;
	output(:,:,3) = outputb;

% 	imwrite(output,[outputpath 'edge/image' sprintf('%04d',imgnum) '.png'],'png');
	imwrite(output,[outputpath 'edge/image' sprintf('%04d',imgnum) '.png'],'png');

end

end

