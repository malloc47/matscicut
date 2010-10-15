function displayResult(imgnum)

series = 1;

img = imread(['data/new/raw/' num2str(series+3) '000_Series/' num2str(series+3) '000_image' sprintf('%04d',imgnum) '.tif']);

labels_raw = dlmread(['image' sprintf('%04d',imgnum) '.labels']);

unique(labels_raw)

labels = uint32(img.*0);

for i = 1:length(labels_raw)
    [x,y] = convertind(i-1,size(labels,2));
    labels(x+1,y+1) = labels_raw(i);
end

% labels = reshape(labels,size(img));

figure; imshow(overlay(img,label2rgb(labels,'jet','w','shuffle'),0.5));

% imwrite(label2rgb(labels,'jet','w','shuffle'),'weird.png','.png');

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