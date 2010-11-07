function watershed_eval(imgnums)

for imgnum = imgnums

	series = 1;
	datapath = 'data/old/scaled/';
	outputpath = 'outputw/';

	img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
	img2 = imhmin(img,20);
	labels = watershed(img2);
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

