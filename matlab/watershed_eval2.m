function watershed_eval2(imgnums,supress)

	source

	if(nargin < 2)
		supress = 3;
    end

	ground = logical(imread(['~/src/projects/matsci/matscicut/' volume 'data/old/scaled/ground/' 'stfl' sprintf('%02d',90) 'alss1th.tif']));
    ground_fg = imerode(~bwmorph(ground,'thin',Inf),strel('disk',10));
%     ground_bg = bwmorph(ground,'thin',Inf);
    
for imgnum = imgnums

	series = 1;
	datapath = 'data/old/scaled/';
	outputpath = 'outputw2/';

	img = imread([volume datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
	img2 = imhmin(img,supress);
    img3 = imimposemin(img2, ground_fg);
	labels = watershed(img3);
	dlmwrite([volume outputpath 'labels/image' sprintf('%04d',imgnum) '.labels'],labels,' ');
	Lrgb = label2rgb(labels,'jet','w','shuffle');
	imwrite(Lrgb,[volume outputpath 'region/image' sprintf('%04d',imgnum) '.png'],'png');
	
	groundbmp = seg2bmap(labels);
	groundbmp = bwmorph(groundbmp,'thin',Inf);
    ground_fg = imerode(~groundbmp,strel('disk',10));
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

	imwrite(output,[volume outputpath 'edge/image' sprintf('%04d',imgnum) '.png'],'png');

end

end

