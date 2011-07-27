function watershed_eval2(imgnums,supress)

	source

	if(nargin < 2)
		supress = 3;
    end

	ground = logical(imread([groundimgpath prefix sprintf('%04d',0) postfix '.' imgtype]));
    ground_fg = imerode(~bwmorph(ground,'thin',Inf),strel('disk',10));
%     ground_bg = bwmorph(ground,'thin',Inf);
    
for imgnum = imgnums

% 	series = 1;
% 	datapath = 'data/old/scaled/';
% 	outputpath = 'outputw2/';

	img = imread([rawpath prefix sprintf('%04d',imgnum) postfix '.' inputimgtype]);
	img2 = imhmin(img,supress);
    img3 = imimposemin(img2, ground_fg);
	labels = watershed(img3);
	dlmwrite([labelpath prefix sprintf('%04d',imgnum) '.' labeltype],labels,' ');
	Lrgb = label2rgb(labels,'jet','w','shuffle');
	imwrite(Lrgb,[regionpath prefix sprintf('%04d',imgnum) '.' imgtype],imgtype);
	
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

	imwrite(output,[edgepath prefix sprintf('%04d',imgnum) '.' imgtype],imgtype);

end

end

