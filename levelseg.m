function levelseg()

%% Boilerplate

addpath(genpath('../../gco/matlab/'));

GCO_MAX_ENERGYTERM = 10000000;

disp('Reading Images');

img0 = imread('4000_image0000.tif');
img1 = imread('4000_image0001.tif');

imgsize = size(img0);

img0labels = img0.*0;

numlabels = 4;

for i = 1:numlabels
    mask = logical(imread(['4000_image0000-mask' num2str(i) '.png']));
    img0labels(mask) = i-1;
end

save('4000_image0000-labels.mat','img0labels');

%     load('4000_image0000-labels.mat');

img0list = img0(:);
img0labelslist = img0labels(:);
img1list = img1(:);

h = GCO_Create(length(img1list),numlabels);

%% Data cost calculation

disp('Computing data cost');

data_cost = ones(numlabels,length(img0list));
data_cost = GCO_MAX_ENERGYTERM .* data_cost;

for i = 1:max(img0labelslist)+1
    dilation = logical(img0labels == i-1);
    dilation = imdilate(dilation,strel('disk',10));
    data_cost(i,dilation(:)) = 1;
end

GCO_SetDataCost(h,data_cost);

%% Smoothness cost calculation

% smooth_cost = zeros(numlabels,numlabels);
% 
% GCO_SetSmoothCost(h,smooth_cost);

%% Neighbor cost calculation

disp('Computing neighbor cost');

neighbor_cost = ones(length(img0list),length(img0list));
neighbor_cost = GCO_MAX_ENERGYTERM .* neighbor_cost;

labelsadj = regionadj(img0labels); 

labelsadj

ioffset = [0  0 1 -1  1  1 -1 -1];
joffset = [1 -1 0  0  1 -1  1 -1];

for i = 1:size(img0,1)
	for j = 1:size(img0,2)
		for n = 1:length(ioffset) % Neighbors
			k=i+ioffset(n);
			l=j+joffset(n);
			if k<1 || l<1 || k>imgsize(1) || l>imgsize(2)
				continue;
			end
			if(img0labels(i,j)~=img0labels(k,l)) % Not same label
				if(labelsadj(img0labels(i,j)+1,img0labels(k,l)+1) == 0) % Not neighboring
					neighbor_cost(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l))=Inf;
				else
					if( abs( img0list(i) - img0list(j) ) < 10 ) % Similar intensities
						neighbor_cost(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l))=10;
					else
						neighbor_cost(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l))=1/(abs(img0list(i) - img0list(j)));
					end
				end
			else
				neighbor_cost(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l))=0;
				
			end
		end
	end
end

for i = 1:size(img0list)
	neighbor_cost(i,i) = 0;
end
GCO_SetNeighbors(h,neighbor_cost);

%% Computatation

disp('Computing labeling');

GCO_Expansion(h);                % Compute optimal labeling via alpha-expansion
labels = GCO_GetLabeling(h);

[E D S] = GCO_ComputeEnergy(h)

GCO_Delete(h);

%      h = GCO_Create(4,3);             % Create new object with NumSites=4, NumLabels=3
%      GCO_SetDataCost(h,[0 9 2 0;      % Sites 1,4 prefer  label 1
%                            3 0 3 3;      % Site  2   prefers label 2 (strongly)
%                            5 9 0 5;]);   % Site  3   prefers label 3
%      GCO_SetSmoothCost(h,[0 1 2;      %
%                              1 0 1;      % Linear (Total Variation) pairwise cost
%                              2 1 0;]);   %
%      GCO_SetNeighbors(h,[0 1 0 0;     % Sites 1 and 2 connected with weight 1
%                             0 0 1 0;     % Sites 2 and 3 connected with weight 1
%                             0 0 0 2;     % Sites 3 and 4 connected with weight 2
%                             0 0 0 0;]);
%      GCO_Expansion(h);                % Compute optimal labeling via alpha-expansion
%      GCO_GetLabeling(h)
%
%      [E D S] = GCO_ComputeEnergy(h)   % Energy = Data Energy + Smooth Energy
%
%      GCO_Delete(h);                   % Delete the GCoptimization object when finished


%      img0 = reshape(img0,imgsize);
%      img1 = reshape(img1,imgsize);

whos

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

end
