function convertSeg(range)
pathin = 'data/new/ground/groundtruth/';
pathout = 'labels/';
for i = range 
	seg = readSeg([pathin sprintf('%04d',i) '.seg']);
	dlmwrite([pathout 'image' sprintf('%04d',i) '.label'],seg,' ');
end
end
