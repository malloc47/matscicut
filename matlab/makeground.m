function makeground()

ground = imread('data/old/hand/stfl90alss1thnew.tif');

ground = imresize(ground,[525 750]);

ground_thin = bwmorph(ground,'thin',Inf);

ground_thin = imdilate(ground_thin,strel('disk',1));

%figure; imshow(ground_thin);

ground_labels = bwlabel(~ground_thin,4);

ground_labels = conditionlabels(ground_labels);
ground_labels = conditionlabels(ground_labels);
ground_labels = conditionlabels(ground_labels);
ground_labels = conditionlabels(ground_labels);

min(min(ground_labels))

%figure; imshow(label2rgb(ground_labels,'jet','w','shuffle'));

dlmwrite('output2/labels/image0090.labels',ground_labels-1,'delimiter',' ');


end
