function evaluate(imgnums)

datapath = 'data/old/scaled/ground/';
ourpath = 'output2/';
wshedpath = 'outputw/';

d=2;

ours  = [];
wshed = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours fmeasure(ground,ours_edge)];
	wshed= [wshed fmeasure(ground,wshed_edge)];

end

figure('visible','off'); 
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
leg = legend('Our Method','Watershed');
set(leg,'Location','SouthEast');
xlabel('Serial Slice');
ylabel('F-measure');
print('-depsc2', 'g.eps');

ours  = [];
wshed = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours precision(ground,ours_edge)];
	wshed= [wshed precision(ground,wshed_edge)];

end

figure('visible','off'); 
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
leg = legend('Our Method','Watershed');
set(leg,'Location','SouthEast');
xlabel('Serial Slice');
ylabel('Precision');
print('-depsc2', 'g2.eps');

ours  = [];
wshed = [];

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));
	ground = bwmorph(ground,'thin',Inf);
	ground = imdilate(ground,strel('disk',d));

	our_label = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed_label = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = bwmorph(logical(seg2bmap(our_label)),'thin',Inf);
	wshed_edge = bwmorph(logical(seg2bmap(wshed_label)),'thin',Inf);
	%figure; imshow(wshed_edge);
	%pause;

	ours = [ours recall(ground,ours_edge)];
	wshed= [wshed recall(ground,wshed_edge)];

end

figure('visible','off'); 
plot(ours,'r.-'); 
hold all
plot(wshed,'g+-');
leg = legend('Our Method','Watershed');
set(leg,'Location','SouthEast');
xlabel('Serial Slice');
ylabel('Recall');
print('-depsc2', 'g3.eps');

figure('visible','off'); 
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
leg = legend('e+j+d+g','j+d+g','d+g','g');
set(leg,'Location','NorthEast');
print('-depsc2', 'h.eps');

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
	num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',d))) ));
end
