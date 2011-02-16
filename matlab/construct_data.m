function construct_data(imgnums)

datapath = 'data/old/scaled/ground/';
ourpath = 'output2/';
wshedpath = 'outputw/';
wshed2path = 'outputw2/';
ncutpath = 'outputn/';
ncut2path = 'outputn2/';
d=4;

% wshedscore = [];
% 
% for i = 1:50
% disp(['Watershed run ' num2str(i)]);
% ours  = [];
% wshed = [];
% watershed_eval2(imgnums,i);
% for imgnum = imgnums
% 
% 	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
% 	ground = bwmorph(ground,'thin',Inf);
% 	ground = imdilate(ground,strel('disk',d));
% 
% 	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
% 	wshed_label = dlmread([wshed2path 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
% 
% 	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
% 	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
% 	%figure; imshow(wshed_edge);
% 	%pause;
% 
% 	ours = [ours fmeasure(ground,ours_edge)];
% 	wshed= [wshed fmeasure(ground,wshed_edge)];
% 
% end
% 
% wshedscore = [wshedscore mean(wshed)];
% 
% end
% 
% [m,idx] = max(wshedscore);
% 
% disp(['Max score    : ' num2str(m)]);
% disp(['Max score idx: ' num2str(idx)]);
% 
% return;
% 
% %%%%%%%%%%

idx = 18;

watershed_eval(imgnums,idx);
watershed_eval2(imgnums,3);

yax = 2:11;

%%% F-measure 

data = cell(6,length(imgnums));

% grd = cell(1,length(imgnums));
% ours  = cell(1,length(imgnums));
% wshed = cell(1,length(imgnums));
% wshed2 = cell(1,length(imgnums));
% ncut = cell(1,length(imgnums));
% ncut2 = cell(1,length(imgnums));

counter = 0;

for imgnum = imgnums
    
    counter = counter + 1;

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) 'gdje.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
    wshed2_label = dlmread([wshed2path 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
    ncut2_label = dlmread([ncut2path 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	wshed_label = conditionlabels(wshed_label);
    wshed2_label = conditionlabels(wshed2_label);
	ncut_label  = conditionlabels(ncut_label);
	ncut2_label  = conditionlabels(ncut2_label);

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
    wshed2_edge = bwmorph(logical(seg2bmap(wshed2_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);
    ncut2_edge = bwmorph(logical(seg2bmap(ncut2_label)),'thin',Inf);

	ours_edge = imdilate(ours_edge,strel('disk',d));
	wshed_edge= imdilate(wshed_edge,strel('disk',d));
    wshed2_edge= imdilate(wshed2_edge,strel('disk',d));
	ncut_edge= imdilate(ncut_edge,strel('disk',d));
	ncut2_edge= imdilate(ncut2_edge,strel('disk',d));

    data{1,counter} = ground;
	data{2,counter} = ours_edge;
    data{3,counter} = wshed2_edge;
	data{4,counter} = wshed_edge;
    data{5,counter} = ncut2_edge;
	data{6,counter} = ncut_edge;
end

save('dat/edge-images.mat','data');

end