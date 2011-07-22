function evaluate2(imgnums)

datapath = 'data/old/scaled/ground/';
ourpath = 'output2/';
wshedpath = 'outputw/';
wshed2path = 'outputw2/';
ncutpath = 'outputn/';
ncut2path = 'outputn2/';
d=4;

yax = 2:11;

%%% F-measure 

load('dat/edge-images.mat');

disp('F-measure');

names = {'Proposed Method','Propagated Watershed','Watershed','Normalized Cut','Normalized Cut 1'};
style = {'r.-','m*-','g+-','b*-','cx-'};

fig = figure('visible','off'); 
hold all;
for i = 2:size(data,1)-1
    list = [];
    for j=1:size(data,2)
        list = [list fmeasure(data{1,j},data{i,j})];
    end
    plot(yax,list,style{1,i-1}); 
end
leg = legend(names);
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('F-measure');
print('-depsc2', 'g.eps');

%%% Precision  

disp('Precision');

fig = figure('visible','off'); 
hold all;
for i = 2:size(data,1)-1
    list = [];
    for j=1:size(data,2)
        list = [list precision(data{1,j},data{i,j})];
    end
    plot(yax,list,style{1,i-1}); 
end
leg = legend(names);
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('Precision');
print('-depsc2', 'g2.eps');

%%% Recall
fig = figure('visible','off'); 
hold all;
for i = 2:size(data,1)-1
    list = [];
    for j=1:size(data,2)
        list = [list recall(data{1,j},data{i,j})];
    end
    plot(yax,list,style{1,i-1}); 
end
leg = legend(names);

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
