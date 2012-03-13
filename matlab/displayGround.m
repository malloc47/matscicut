function displayGround(imgnums)

source

for imgnum = imgnums

series = 1;

%datapath = 'data/old/scaled/';
%datapath2 = 'data/old/scaled/ground/';
%outputpath = 'outputg/';

img = imread([rawpath prefix sprintf('%04d',imgnum) postfix '.' inputimgtype]);

gt = imread([groundpath prefix sprintf('%04d',imgnum) postfix '.' inputimgtype]);

%img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
%gt = imread([datapath2 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']);

groundbmp = bwmorph(gt,'thin',Inf);
groundbmp = imdilate(groundbmp,strel('disk',2));

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

imwrite(output,[outputpath 'ground/image' sprintf('%04d',imgnum) '.png'],'png');


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
