function watershed_eval3_intensity(src,target,d,supress)

	source

	if(nargin < 3)
		d = 10;
    end

	if(nargin < 4)
		supress = 3;
    end

	%ground = logical(imread([w1labelpath prefix sprintf('%04d',src) postfix '.' imgtype]));
	ground_labels = dlmread([w1labelpath prefix sprintf('%04d',src) postfix '.' labeltype]);
	groundbmp = seg2bmap(ground_labels);
    ground_fg = imerode(~bwmorph(groundbmp,'thin',Inf),strel('disk',d));
	groundbmp = imdilate(groundbmp,strel('disk',2));

	img = imread([rawpath prefix sprintf('%04d',target) postfix '.' inputimgtype]);
	img2 = imhmin(compute_grad_mag(img),supress);
    img3 = imimposemin(img2, ground_fg);
	labels = watershed(img3);
	dlmwrite([w1labelpath prefix sprintf('%04d',target) '.' labeltype],labels,' ');
	Lrgb = label2rgb(labels,'jet','w','shuffle');
	imwrite(Lrgb,[w1regionpath prefix sprintf('%04d',target) '.' imgtype],imgtype);
	
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

	imwrite(output,[w1edgepath prefix sprintf('%04d',target) '.' imgtype],imgtype);

end

function gradmag = compute_grad_mag(I)
    % http://www.mathworks.com/products/image/demos.html?file=/products/demos/shipping/images/ipexwatershed.html
    hy = fspecial('sobel');
    hx = hy';
    Iy = imfilter(double(I), hy, 'replicate');
    Ix = imfilter(double(I), hx, 'replicate');
    gradmag = sqrt(Ix.^2 + Iy.^2);
end
