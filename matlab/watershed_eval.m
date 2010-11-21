function watershed_eval(imgnums,supress)

	if(nargin < 2)
		supress = 20;
	end

for imgnum = imgnums

	series = 1;
	datapath = 'data/old/scaled/';
	outputpath = 'outputw/';

	img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
	img2 = imhmin(img,supress);
	labels = watershed(img2);
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

	imwrite(output,[outputpath 'edge/image' sprintf('%04d',imgnum) '.png'],'png');

end

end

