function evaluate(imgnums)

datapath = 'data/old/scaled/ground/';
ourpath = 'output2/';
wshedpath = 'outputw/';
ncutpath = 'outputn/';
d=3;

%wshedscore = [];

%for i = 1:50
%disp(['Watershed run ' num2str(i)]);
%ours  = [];
%wshed = [];
%watershed_eval(imgnums,i);
%for imgnum = imgnums

	%ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	%ground = bwmorph(ground,'thin',Inf);
	%ground = imdilate(ground,strel('disk',d));

	%our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	%wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	%ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	%wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	%%figure; imshow(wshed_edge);
	%%pause;

	%ours = [ours fmeasure(ground,ours_edge)];
	%wshed= [wshed fmeasure(ground,wshed_edge)];

%end

%wshedscore = [wshedscore mean(wshed)];


%end

%[m,idx] = max(wshedscore);

%disp(['Max score    : ' num2str(m)]);
%disp(['Max score idx: ' num2str(idx)]);

idx = 18;

watershed_eval(imgnums,idx);

ours  = [];
wshed = [];
ncut = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours fmeasure(ground,ours_edge)];
	wshed= [wshed fmeasure(ground,wshed_edge)];
	ncut= [ncut fmeasure(ground,ncut_edge)];

end

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
plot(ncut,'b*-');
leg = legend('Our Method','Watershed','Normalized Cut');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('F-measure');
print('-depsc2', 'g.eps');

ours  = [];
wshed = [];
ncut = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours precision(ground,ours_edge)];
	wshed= [wshed precision(ground,wshed_edge)];
	ncut= [ncut precision(ground,ncut_edge)];

end

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
plot(ncut,'b*-');
leg = legend('Our Method','Watershed','Normalized Cut');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('Precision');
print('-depsc2', 'g2.eps');

ours  = [];
wshed = [];
ncut = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	ncut_label = dlmread([ncutpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	ncut_edge = bwmorph(logical(seg2bmap(ncut_label)),'thin',Inf);
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours recall(ground,ours_edge)];
	wshed= [wshed recall(ground,wshed_edge)];
	ncut= [ncut recall(ground,ncut_edge)];

end

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
plot(ncut,'b*-');
leg = legend('Our Method','Watershed','Normalized Cut');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('Recall');
print('-depsc2', 'g3.eps');

fig = figure('visible','off'); 
%set(fig, 'PaperUnits', 'inches');
%set(fig, 'PaperSize', [1 1]);
hold all
xlabel('Serial Slice');
ylabel('F-measure');
%colors = ['r','b','g','m'];
colors = ['m','g','b','r'];
%style = ['s','d','*','o'];
style = ['o','*','d','s'];
index = 0;
%for i = ['g','d','j','e']
for i = ['e','j','d','g']
	index = index + 1;
	ours = [];
	for imgnum = imgnums

		ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
		ground = bwmorph(ground,'thin',Inf);
		ground = imdilate(ground,strel('disk',d));

		our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '-' i '.labels'],' ');
		our_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	
		ours = [ours fmeasure(ground,our_edge)];
	end
	plot(ours,[colors(index) style(index) '-']);
end
%leg = legend('e+j+d+g','j+d+g','d+g','g');
leg = legend('Edge+Junctions+Deletion+Global','Junctions+Deletion+Global','Deletion+Global','Global');
leg = legend('Strategy III + Strategy II + Strategy I + Global','Strategy II + Strategy I + Global','Strategy I + Global','Global');
set(leg,'Location','NorthEast');
set(leg,'FontSize',12);
print('-depsc2', 'h.eps');


ours  = [];
wshed = [];
%groundnum = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	ground_label = bwlabel(~ground,4);
	ground_num = max(max(ground_label));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	our_num = max(max(our_label));
	wshed_num = max(max(wshed_label));

	ours = [ours abs(our_num-ground_num) ];
	wshed= [wshed abs(wshed_num-ground_num) ];
	%groundnum = [groundnum max(max(ground_label))];

end

fig = figure('visible','off'); 
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
%plot(groundnum,'bs-');
leg = legend('Our Method','Watershed');
set(leg,'Location','SouthEast');
set(leg,'FontSize',12);
xlabel('Serial Slice');
ylabel('Number of labels');
print('-depsc2', 'j.eps');

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
	d=3;
	%num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',5))) ));
	num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',d))) ));
end
