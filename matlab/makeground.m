function makeground(num,supressback)

source

if(nargin<2)
	supressback = true;
end

ground = logical(rgb2gray(imread([groundpath prefix sprintf('%04d',num) postfix '.' imgtype])));

if(supressback)
	ground_thin = bwmorph(ground,'thin',Inf);
	ground_thin = imdilate(ground_thin,strel('disk',1));
	ground_labels = bwlabel(~ground_thin,4);
else
	ground_labels = bwlabel(ground,4);
end

%ground_labels = conditionlabels(ground_labels);

sizes = [];

for i = 0:max(max(ground_labels))
	fprintf('%g ',sum(sum(ground_labels == i)))
	sizes = [sizes sum(sum(ground_labels == i))];
end

min(sizes)
max(sizes)

min(min(ground_labels))
max(max(ground_labels))

%figure; imshow(label2rgb(ground_labels,'jet','w','shuffle'));

if(supressback)
	dlmwrite([volume 'ground/' prefix sprintf('%04d',num) postfix '.' labeltype],ground_labels-1,'delimiter',' ');
else
	dlmwrite([volume 'ground/' prefix sprintf('%04d',num) postfix '.' labeltype],ground_labels,'delimiter',' ');
end

end
