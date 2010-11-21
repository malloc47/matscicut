function thinImg(imgnums)

for imgnum = imgnums

    datapath = 'data/old/scaled/';
    outputpath = 'output2/';
    
    img = imread([datapath 'stfl' sprintf('%02d',imgnum) 'alss1.tif']);
    
    imgbw = im2bw(img,graythresh(img));
    
    imgbw = bwmorph(imgbw,'thin',Inf);
    
    imwrite(imgbw,[outputpath 'maps/image' sprintf('%04d',imgnum) '.tif'],'tif');
    
    figure; imshow(imgbw);
    
end


end