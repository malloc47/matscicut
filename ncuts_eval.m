function ncuts_eval(imgnums)

	%addpath('../../programs/normalized-cuts/ncut_multiscale_1_6/');

	init

for imgnum = imgnums

	series = 1;
	datapath = '~/src/projects/matscicut/data/old/scaled/';
	outputpath = '~/src/projects/matscicut/outputn/';

	ground = logical(imread(['~/src/projects/matscicut/data/old/scaled/ground/' 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground_label = bwlabel(~ground,4);
	ground_num = max(max(ground_label))

	img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
	%img2 = imhmin(img,20);
	[labels,X,lambda,Xr,W,C,timing] = ncut_multiscale(img,ground_num);
	%labels = watershed(img2);
	dlmwrite([outputpath 'labels/image' sprintf('%04d',imgnum) '.labels'],labels,' ');
	Lrgb = label2rgb(labels,'jet','w','shuffle');
	imwrite(Lrgb,[outputpath 'region/image' sprintf('%04d',imgnum) '.png'],'png');
	
	groundbmp = seg2bmap(labels);

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

	imwrite(output,[outputpath 'edge/image' sprintf('%04d',imgnum) '.png'],'png');

end

end

