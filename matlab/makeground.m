function makeground(num)

ground = imread(['seq1/img/ground/stfl' int2str(num) 'alss1th.tif']);

%ground = imresize(ground,[525 750]);

ground_thin = bwmorph(ground,'thin',Inf);

ground_thin = imdilate(ground_thin,strel('disk',1));

%figure; imshow(ground_thin);

ground_labels = bwlabel(~ground_thin,4);

disp('Condition 1');
ground_labels = conditionlabels(ground_labels);
disp('Condition 2');
ground_labels = conditionlabels(ground_labels);
disp('Condition 3');
ground_labels = conditionlabels(ground_labels);
disp('Condition 4');
ground_labels = conditionlabels(ground_labels);

min(min(ground_labels))

%figure; imshow(label2rgb(ground_labels,'jet','w','shuffle'));

dlmwrite(['seq1/ground/image00' int2str(num) '.labels'],ground_labels-1,'delimiter',' ');

end
