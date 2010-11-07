function evaluate(imgnums)

datapath = 'data/old/scaled/ground/';
ourpath = 'output2/';
wshedpath = 'outputw/';

for imgnum = imgnums

	ground = logical(imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1th.tif']));

	ours = dlmread([ourpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');
	wshed = dlmread([wshedpath 'labels/image' sprintf('%04d',imgnum) '.labels'],' ');

	ours_edge = logical(seg2bmap(ours));
	wshed_edge = logical(seg2bmap(wshed));

	score1 = fmeasure(ground,ours_edge)
	score2 = fmeasure(ground,wshed_edge)

end
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
	num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',5))) ));
end
