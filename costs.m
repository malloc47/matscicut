function labelsadj = costs(imgnum)

%% Boilerplate

tic;

% addpath(genpath('../../programs/gco/'));

GCO_MAX_ENERGYTERM = 10000000;
%GCO_MAX_ENERGYTERM = 3162;
DILATE_AMOUNT=10;

datapath = 'data/new/raw/'

disp('Reading Images');

for i = 1:4
    img(:,:,i) = imread([datapath num2str(i+3) '000_Series/' num2str(i+3) '000_image' sprintf('%04d',imgnum) '.tif']);
end

% imgseed = imread(['data/new/ground/image' sprintf('%04d',imgnum) '.pbm']);

load(['data/new/ground/' 'image' sprintf('%04d',imgnum) '.mat']);

for i = 1:4
    imgn(:,:,i) = imread([datapath num2str(i+3) '000_Series/' num2str(i+3) '000_image' sprintf('%04d',imgnum+1) '.tif']);
end

imgsize = size(img(:,:,1));
pixels = imgsize(1)*imgsize(2);
numlabels = max(max(imglabels));

disp(['Number of labels: ' int2str(max(max(imglabels)))]);

% data_cost = int32(ones(numlabels,pixels).*GCO_MAX_ENERGYTERM);
%smooth_cost = int32(ones(numlabels,numlabels)) .* int32((GCO_MAX_ENERGYTERM));

%data = int32(ones(numlabels*pixels,1).*GCO_MAX_ENERGYTERM);

%% Data Cost

%disp('Computing data costs');

%unique(imglabels)

%total = 0;
%max_reg = 0;

%composite_layers = imglabels.*0;

%for l = 1:numlabels+1
%%    disp(int2str(l));
    %dilation = logical(imglabels == l); % Get region
%%    dilation = imdilate(dilation,strel('disk',DILATE_AMOUNT)); % Dilate
	%composite_layers = composite_layers | dilation;
	%total = total + sum(sum(dilation));
	%max_reg = max(max_reg,max(max(bwlabel(dilation))));
	%%imwrite(dilation,['labels/label' sprintf('%04d',l-1) '.png'],'png');
	%for i = 1:imgsize(1)
		%for j = 1:imgsize(2)
			%if dilation(i,j) == 1
				%data( ((j-1)+(i-1)*imgsize(2))*numlabels + (l-1) +1 ) = 1;
			%end
		%end
	%end

%end

%whos
%total
%max_reg
%sum(sum(composite_layers))


%dlmwrite(['data/new/intermediate/image' sprintf('%04d',imgnum) '.data' ],data);

%% Smooth Cost

%disp('Computing smoothness costs');

%labelsadj = regionadj(imglabels); 

%c = 10;

%for i = 1:numlabels
	%for j = 1:numlabels
		%if i==j % Same label
			%smooth_cost(i,j)=0;	
			%smooth_cost(j,i)=0;	
		%elseif labelsadj(i,j) == 1 % Labels are adjacent
			%smooth_cost(i,j)=c;	
			%smooth_cost(j,i)=c;		
		%end % Don't do anything else since it is already inited to Inf
	%end
%end

%% dlmwrite(['data/new/intermediate/image' sprintf('%04d',imgnum) '.adj' ],labelsadj,'delimiter',' ');

%return;

disp(toc);

end

function adj = regionadj(labels)
	numlabels = max(max(labels));

	adj = zeros(numlabels,numlabels);
	
	for i = 1:size(labels,1)-1
		for j = 1:size(labels,2)
			if(labels(i,j)~=labels(i+1,j))
				adj(labels(i,j)+1,labels(i+1,j)+1) = 1;
				adj(labels(i+1,j)+1,labels(i,j)+1) = 1;
			end
		end
	end

	for j = 1:size(labels,2)-1
		for i = 1:size(labels,1)
			if(labels(i,j)~=labels(i,j+1))
				adj(labels(i,j)+1,labels(i,j+1)+1) = 1;
				adj(labels(i,j+1)+1,labels(i,j)+1) = 1;
			end
		end
	end

	for i = 2:size(labels,1)
		for j = 1:size(labels,2)
			if(labels(i,j)~=labels(i-1,j))
				adj(labels(i,j)+1,labels(i-1,j)+1) = 1;
				adj(labels(i-1,j)+1,labels(i,j)+1) = 1;
			end
		end
	end

	for j = 2:size(labels,2)
		for i = 1:size(labels,1)
			if(labels(i,j)~=labels(i,j-1))
				adj(labels(i,j)+1,labels(i,j-1)+1) = 1;
				adj(labels(i,j-1)+1,labels(i,j)+1) = 1;
			end
		end
	end


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

function new_labels = conditionlabels(labels)

new_labels = labels;

for i = 1:size(labels,1)
    for j = 1:size(labels,2)
        if labels(i,j) == 0
            if i~=size(labels,1) && labels(i+1,j) ~= 0
                new_labels(i,j) = labels(i+1,j);
            elseif j~=size(labels,2) && labels(i,j+1) ~= 0
                new_labels(i,j) = labels(i,j+1);
            elseif i~=1 && labels(i-1,j) ~= 0
                new_labels(i,j) = labels(i-1,j);
            elseif j~=1 && labels(i,j-1) ~= 0
                new_labels(i,j) = labels(i,j-1);
            else
                disp('Complaining');
            end
        end
    end
end
end
