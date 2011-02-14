function evaluate(imgnums)

datapath = 'data/old/scaled/ground/';
ourpath = 'output2/';
wshedpath = 'outputw/';
wshed2path = 'outputw2/';
ncutpath = 'outputn/';
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

ours  = [];
wshed = [];
wshed2 = [];
ncut = [];

disp('F-measure');

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) 'gdje.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
    wshed2_label = dlmread([wshed2path 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	wshed_label = conditionlabels(wshed_label);
    wshed2_label = conditionlabels(wshed2_label);
	ncut_label  = conditionlabels(ncut_label);

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
    wshed2_edge = bwmorph(logical(seg2bmap(wshed2_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);

	ours_edge = imdilate(ours_edge,strel('disk',d));
	wshed_edge= imdilate(wshed_edge,strel('disk',d));
    wshed2_edge= imdilate(wshed2_edge,strel('disk',d));
	ncut_edge= imdilate(ncut_edge,strel('disk',d));
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours fmeasure(ground,ours_edge)];
	wshed= [wshed fmeasure(ground,wshed_edge)];
    wshed2= [wshed2 fmeasure(ground,wshed2_edge)];
	ncut= [ncut fmeasure(ground,ncut_edge)];

end

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
plot(yax,ours,'r.-'); 
dlmwrite('ours-f.dat', [yax' ours'],' ');
hold all
plot(yax,wshed2,'m*-');
dlmwrite('wshed2-f.dat', [yax' wshed2'],' ');
plot(yax,wshed,'g+-');
dlmwrite('wshed-f.dat', [yax' wshed'],' ');
plot(yax,ncut,'b*-');
dlmwrite('ncut-f.dat', [yax' ncut'],' ');
leg = legend('Proposed Method','Watershed','Propagated Watershed','Normalized Cut');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('F-measure');
print('-depsc2', 'g.eps');

%%% Precision  

ours  = [];
wshed = [];
ncut = [];

disp('Precision');

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	wshed_label = conditionlabels(wshed_label);
	ncut_label  = conditionlabels(ncut_label);

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);

	ours_edge = imdilate(ours_edge,strel('disk',d));
	wshed_edge= imdilate(wshed_edge,strel('disk',d));
	ncut_edge= imdilate(ncut_edge,strel('disk',d));

	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours precision(ground,ours_edge)];
	wshed= [wshed precision(ground,wshed_edge)];
	ncut= [ncut precision(ground,ncut_edge)];

end

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
plot(yax,ours,'r.-'); 
hold all
plot(yax,wshed,'g+-');
plot(yax,ncut,'b*-');
leg = legend('Proposed Method','Watershed','Normalized Cut');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('Precision');
print('-depsc2', 'g2.eps');


%%% Recall

ours  = [];
wshed = [];
ncut = [];

disp('Recall');

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	wshed_label = conditionlabels(wshed_label);
	ncut_label  = conditionlabels(ncut_label);

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);

	ours_edge = imdilate(ours_edge,strel('disk',d));
	wshed_edge= imdilate(wshed_edge,strel('disk',d));
	ncut_edge= imdilate(ncut_edge,strel('disk',d));
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours recall(ground,ours_edge)];
	wshed= [wshed recall(ground,wshed_edge)];
	ncut= [ncut recall(ground,ncut_edge)];

end

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
plot(yax,ours,'r.-'); 
hold all
plot(yax,wshed,'g+-');
plot(yax,ncut,'b*-');
leg = legend('Proposed Method','Watershed','Normalized Cut');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('Recall');
print('-depsc2', 'g3.eps');

%%% Self comparision

disp('Self');

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
hold all
xlabel('Serial Slice');
ylabel('F-measure');
%colors = ['r','b','g','m'];
%colors = ['m','g','b','r'];
%colors = ['m','g','b','r','y','c','k','w'];
%style = ['s','d','*','o'];
style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];
index = 0;
%for i = ['g','d','j','e']
%for i = ['gdje','gde','gje','gdj', 'gd','gj','ge','g']
%str = {'gdje' 'gde' 'gje' 'gdj' 'gd' 'gj' 'ge' 'g'};
str = {'gdje' 'gje' 'gde' 'ge' 'gdj' 'gj' 'gd' 'g'};
for i = 1:8 
	%index = index + 1;
	ours = [];
	for imgnum = imgnums

		ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
		ground = bwmorph(ground,'thin',Inf);
		ground = imdilate(ground,strel('disk',d));

		our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) str{1,i} '.labels'],' ');
		our_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
		our_edge = imdilate(our_edge,strel('disk',d));
	
		ours = [ours fmeasure(ground,our_edge)];
	end
	%plot(ours,[colors(index) style(index) '-']);
	%plot(ours,[colors(i) style(i) '-']);
	plot(yax,ours,[style(i) '-']);
end
%leg = legend('e+j+d+g','j+d+g','d+g','g');
%leg = legend('Edge+Junctions+Deletion+Global','Junctions+Deletion+Global','Deletion+Global','Global');
%leg = legend('Strategy III + Strategy II + Strategy I + Global','Strategy II + Strategy I + Global','Strategy I + Global','Global','Global + Strategy II + Strategy III');
%leg = legend('Strategy I + Strategy II + Strategy III','Strategy I + Strategy III','Strategy II + Strategy III','Strategy I','Strategy II','Strategy III','Global');
leg = legend('Strategy I, II, III', ...
		'Strategy II, III', ... 
		'Strategy I, III', ...
		'Strategy III', ... 
		'Strategy I, II', ...
		'Strategy II', ... 
		'Strategy I', ... 
		'Global');
%set(leg,'Location','NorthEast');
set(leg,'Location','SouthWest');
set(leg,'FontSize',12);
print('-depsc2', 'h.eps');


%%% Self comparision

disp('Self2');

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
hold all
xlabel('Serial Slice');
ylabel('F-measure');
%colors = ['r','b','g','m'];
%colors = ['m','g','b','r'];
%colors = ['m','g','b','r','y','c','k','w'];
%style = ['s','d','*','o'];
style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];
index = 0;
%for i = ['g','d','j','e']
%for i = ['gdje','gde','gje','gdj', 'gd','gj','ge','g']
%str = {'gdje' 'gde' 'gje' 'gdj' 'gd' 'gj' 'ge' 'g'};
str = {'gdje' 'g'};
for i = 1:2 
	%index = index + 1;
	ours = [];
	for imgnum = imgnums

		ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
		ground = bwmorph(ground,'thin',Inf);
		ground = imdilate(ground,strel('disk',d));

		our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) str{1,i} '.labels'],' ');
		our_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
		our_edge = imdilate(our_edge,strel('disk',d));
	
		ours = [ours fmeasure(ground,our_edge)];
	end
	%plot(ours,[colors(index) style(index) '-']);
	%plot(ours,[colors(i) style(i) '-']);
	plot(yax,ours,[style(i) '-']);
end
%leg = legend('e+j+d+g','j+d+g','d+g','g');
%leg = legend('Edge+Junctions+Deletion+Global','Junctions+Deletion+Global','Deletion+Global','Global');
%leg = legend('Strategy III + Strategy II + Strategy I + Global','Strategy II + Strategy I + Global','Strategy I + Global','Global','Global + Strategy II + Strategy III');
%leg = legend('Strategy I + Strategy II + Strategy III','Strategy I + Strategy III','Strategy II + Strategy III','Strategy I','Strategy II','Strategy III','Global');
leg = legend('Global+Local', ...
		'Global Only');
%set(leg,'Location','NorthEast');
set(leg,'Location','SouthWest');
set(leg,'FontSize',12);
box on;
print('-depsc2', 'r.eps');

%ours  = [];
%wshed = [];
%%groundnum = [];

%for imgnum = imgnums

	%ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	%ground = bwmorph(ground,'thin',Inf);
	%ground = imdilate(ground,strel('disk',d));

	%ground_label = bwlabel(~ground,4);
	%ground_num = max(max(ground_label));

	%our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	%wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	%our_num = max(max(our_label));
	%wshed_num = max(max(wshed_label));

	%ours = [ours abs(our_num-ground_num) ];
	%wshed= [wshed abs(wshed_num-ground_num) ];
	%%groundnum = [groundnum max(max(ground_label))];

%end

%fig = figure('visible','off'); 
%plot(yax,ours,'r.-'); 
%hold all
%plot(yax,wshed,'g+-');
%%plot(yax,groundnum,'bs-');
%leg = legend('Proposed Method','Watershed');
%set(leg,'Location','SouthEast');
%set(leg,'FontSize',12);
%xlabel('Serial Slice');
%ylabel('Number of labels');
%print('-depsc2', 'j.eps');

end

function num = fmeasure(gt,ev)
	num = 2*((precision(gt,ev) * recall(gt,ev)) / (precision(gt,ev) + recall(gt,ev)));
end

function num = precision(gt,ev)
	num = tp(gt,ev)/(tp(gt,ev)+fp(gt,ev));
end

function num = recall(gt,ev)
	num = tp(gt,ev)/(tp(gt,ev)+fn(gt,ev));
end

function num = tp(gt,ev)
	num = sum(sum( gt & ev ));
end

function num = fp(gt,ev)
	num = sum(sum( ~gt & ev ));
end

function num = fn(gt,ev)
	d=2;
	%num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',5))) ));
	%num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',d))) ));
	num = sum(sum( gt & (~ev) ));
end
