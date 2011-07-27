function convertSeg(range)
pathin = 'seq2/ground/';
pathout = 'seq2/ground/';
imgpathout= 'seq2/ground/img/';
for i = range 
	seg = readSeg([pathin sprintf('%04d',i) '.seg']);
	dlmwrite([pathout 'image' sprintf('%04d',i) '.label'],seg,' ');
    imwrite(seg2bmap(seg),[imgpathout 'image' sprintf('%04d',i) '.png'],'png');
end
end
