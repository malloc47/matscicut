function label_to_img(label_path,img_path,out_path)
    img = imread(img_path);
    label = dlmread(label_path,' ');
    label = reshape(label,size(img));
    groundbmp = seg2bmap(label);
    groundbmp = bwmorph(groundbmp,'thin',Inf);
    
    output(:,:,1) = img;
    output(:,:,2) = img;
    output(:,:,3) = img;
    
    outputr = output(:,:,1);
    outputg = output(:,:,2);
    outputb = output(:,:,3);
    
    outputr(groundbmp) = 255;
    outputg(groundbmp) = 0;
    outputb(groundbmp) = 0;
    
    output(:,:,1) = outputr;
    output(:,:,2) = outputg;
    output(:,:,3) = outputb;
    
    imwrite(output,out_path,'png');
end