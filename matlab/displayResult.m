function displayResult(imgnums)

source

for imgnum = imgnums

series = 1;

%img = imread([volume datapath num2str(series+3) '000_Series/' num2str(series+3) '000_image' sprintf('%04d',imgnum) '.tif']);
img = imread([rawpath prefix sprintf('%04d',imgnum) postfix '.' inputimgtype]);
%img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);

% ground = readSeg(['evaluation/groundtruth/' sprintf('%04d',imgnum) '.seg']);
% groundbmp = seg2bmap(ground);

%labels = dlmread([outputpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
labels = dlmread([labelpath 'image' sprintf('%04d',imgnum) '.' labeltype],' ');

disp([int2str(imgnum) ':' int2str(length(unique(labels)))]);

%labels_raw = labels_raw(:);

%length(unique(labels_raw))

%labels = uint32(img.*0);

%for i = 1:length(labels_raw)
	%[y,x] = convertind(i-1,size(labels,2));
	%labels(x+1,y+1) = labels_raw(i);
%end

labels = reshape(labels,size(img));
% labels = readSeg(['evaluation/groundtruth/' sprintf('%04d',imgnum) '.seg']);

groundbmp = seg2bmap(labels);
groundbmp = bwmorph(groundbmp,'thin',Inf);
%groundbmp = imdilate(groundbmp,strel('disk',2));

% figure; imshow(overlay(img,label2rgb(labels,'jet','w','shuffle'),0.5));

%load labelmap;

%imwrite(overlay(img,label2rgb(labels,'jet','w','shuffle'),0.5),[outputpath 'overlay/image' sprintf('%04d',imgnum) '.png'],'png');

output(:,:,1) = img; %label2rgb(labels,'jet','w','shuffle');
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

% figure; imshow(output);

imwrite(output,[outputpath 'image' sprintf('%04d',imgnum) '.' imgtype],imgtype);

% imwrite(label2rgb(labels,'jet','w','shuffle'),'weird.png','.png');

%if(iter==0)
	%img_prev = imread(['data/new/raw/' num2str(series+3) '000_Series/' num2str(series+3) '000_image' sprintf('%04d',imgnum-1) '.tif']);
	%imwrite(overlay(img_prev,label2rgb(labels,labelcolors,'w'),0.5),['output/image' sprintf('%04d',imgnum-1) '-s.png'],'png');
%end

end

end

function [x,y] = convertind(ind,w)
x=mod(ind,w);
y=(ind-x)/w;
end

function final_overlay = overlay(img1,img2,alpha)

if nargin < 3
    alpha = 0.5;
end

img1(:,:,2) = img1(:,:,1);
img1(:,:,3) = img1(:,:,1);

final_overlay = img1;
final_overlay(:,:,1) = alpha.*img1(:,:,1) + (1-alpha).*img2(:,:,1);
final_overlay(:,:,2) = alpha.*img1(:,:,2) + (1-alpha).*img2(:,:,2);
final_overlay(:,:,3) = alpha.*img1(:,:,3) + (1-alpha).*img2(:,:,3);
end
