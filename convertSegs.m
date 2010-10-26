function convertSegs()

for i = 1:40
dlmwrite(['evaluation/groundtruth/' sprintf('%04d',i) '.labels'],readSeg(['evaluation/groundtruth/' sprintf('%04d',i) '.seg']),'delimiter',' ');
end

end
