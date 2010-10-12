function matseg(imgnum)

%% Boilerplate

addpath(genpath('../../programs/gco/'));

%GCO_MAX_ENERGYTERM = 10000000;
GCO_MAX_ENERGYTERM = 3162;
DILATE_AMOUNT=10;

disp('Reading Images');

for i = 1:4
    img(:,:,i) = imread(['data/new/raw/' num2str(i+3) '000_Series/' num2str(i+3) '000_image' sprintf('%04d',imgnum) '.tif']);
end

% imgseed = imread(['data/new/ground/image' sprintf('%04d',imgnum) '.pbm']);

load(['data/new/ground/image' sprintf('%04d',imgnum) '.mat']);

for i = 1:4
    imgn(:,:,i) = imread(['data/new/raw/' num2str(i+3) '000_Series/' num2str(i+3) '000_image' sprintf('%04d',imgnum+1) '.tif']);
end

imgsize = size(img(:,:,1));
pixels = imgsize(1)*imgsize(2);
numlabels = max(max(imglabels));

% imgseed = bwmorph(imgseed,'thin',Inf);
% 
% imglabels = bwlabel(~imgseed,4);
% 
% imglabels = conditionlabels(imglabels);
% imglabels = conditionlabels(imglabels);
% 
% save('data/new/ground/image0000.mat','imglabels');

% figure; imshow(label2rgb(imglabels,'jet','w','shuffle'));

disp(['Number of labels: ' int2str(max(max(imglabels)))]);

data_cost = int32(ones(numlabels,pixels).*GCO_MAX_ENERGYTERM);
neighbor_cost = sparse(pixels,pixels);
smooth_cost = int32(ones(numlabels,numlabels)) .* int32((GCO_MAX_ENERGYTERM));

h = GCO_Create(pixels,numlabels);

%% Data Cost

disp('Computing data costs');

for i = 1:numlabels
%     disp(int2str(i));
    dilation = logical(imglabels == i); % Get region
    dilation = imdilate(dilation,strel('disk',DILATE_AMOUNT)); % Dilate
    data_cost(i,dilation(:)) = 1; % Use dilated region to set 1s
end

GCO_SetDataCost(h,data_cost);

%% Smooth Cost

disp('Computing smoothness costs');

labelsadj = regionadj(imglabels); 

c = 10;

for i = 1:numlabels
	for j = 1:numlabels
		if i==j % Same label
			smooth_cost(i,j)=0;	
			smooth_cost(j,i)=0;	
		elseif labelsadj(i,j) == 1 % Labels are adjacent
			smooth_cost(i,j)=c;	
			smooth_cost(j,i)=c;		
		end % Don't do anything else since it is already inited to Inf
	end
end

GCO_SetSmoothCost(h,smooth_cost);

%% Neighbor cost

disp('Computing neighbor costs');

ioffset = [0  0 1 -1  1  1 -1 -1];
joffset = [1 -1 0  0  1 -1  1 -1];

% for count = 1:pixels
%     disp(int2str(i));
% end

for j = 1:imgsize(2)
    for i = 1:imgsize(1)
        disp(int2str(sub2ind(imgsize,i,j)));
        for n = 1:length(ioffset)
            k=i+ioffset(n);
            l=j+joffset(n);
            if k<1 || l<1 || k>imgsize(1) || l>imgsize(2) ; continue; end
            w = round((1/(double(abs(int32(imgn(i,j,1)) - int32(imgn(k,l,1))))+1))*GCO_MAX_ENERGYTERM);
            
            if w<=0 ; continue; end
            
            idx1 = min(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l));
            idx2 = max(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l));
            
            neighbor_cost(idx1,idx2)=w;
        end
    end
end


% neighbor_cost = double(int32(round(neighbor_cost)));
GCO_SetNeighbors(h,neighbor_cost);

%% Labeling

disp('Computing labeling');

GCO_Expansion(h);
new_labels = GCO_GetLabeling(h);

new_labels = reshape(new_labels,imgsize);

imwrite(overlay(img(:,:,1),label2rgb(new_labels,'jet','w','shuffle')),'new_labels.png','png');
% imwrite(label2rgb(img0labels+1,'jet','w','shuffle'),'old_labels.png','png');

[E D S] = GCO_ComputeEnergy(h)


%% Done

whos

GCO_Delete(h);

return;

%% Old



%% Neighbor cost calculation

disp('Computing neighbor cost');

neighbor_cost = zeros(length(img0list),length(img0list));

ioffset = [0  0 1 -1  1  1 -1 -1];
joffset = [1 -1 0  0  1 -1  1 -1];

for i = 1:size(img0,1)
	for j = 1:size(img0,2)
		for n = 1:length(ioffset)
			neighbor_cost(sub2ind(imgsize,i,j),sub2ind(imgsize,i,j))=0;
			k=i+ioffset(n);
			l=j+joffset(n);
			if k<1 || l<1 || k>imgsize(1) || l>imgsize(2)
				continue;
			end
			
			w = (1/(double(abs(int32(img0(i,j)) - int32(img0(k,l))))+1))*GCO_MAX_ENERGYTERM;

			neighbor_cost(sub2ind(imgsize,i,j),sub2ind(imgsize,k,l))=w;
			neighbor_cost(sub2ind(imgsize,k,l),sub2ind(imgsize,i,j))=w;
		end
	end
end


% neighbor_cost(find(neighbor_cost < GCO_MAX_ENERGYTERM)) .* GCO_MAX_ENERGYTERM;
neighbor_cost = double(int32(round(neighbor_cost)));

% dlmwrite('neighborcost.txt',neighbor_cost);

GCO_SetNeighbors(h,neighbor_cost);


%% Computatation

disp('Computing labeling');

GCO_Expansion(h);                % Compute optimal labeling via alpha-expansion
labels = GCO_GetLabeling(h);

new_labels = reshape(labels,imgsize);


imwrite(label2rgb(new_labels,'jet','w','shuffle'),'new_labels.png','png');
imwrite(label2rgb(img0labels+1,'jet','w','shuffle'),'old_labels.png','png');

[E D S] = GCO_ComputeEnergy(h)

GCO_Delete(h);



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