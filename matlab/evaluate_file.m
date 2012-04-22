function evaluate_file(ground_file,file,outfile,dists)
    if(nargin < 4)
        dists=3;
    end    
    result = [];
    ground = read_file(ground_file);
    img = read_file(file);
% $$$     unique(dlmread(ground_file,' '))
% $$$     length(unique(dlmread(ground_file,' ')))
% $$$     unique(dlmread(file,' '))
% $$$     length(unique(dlmread(file,' ')))
    size_diff = length(unique(dlmread(ground_file,' '))) - ...
        length(unique(dlmread(file,' ')));
    for d = dists
        [f,p,r] = fmeasure(ground,img,d);
        result = [result; f p r size_diff];
    end
    dlmwrite(outfile,result,',');
end

function img = read_file(input)
% $$$     img = bwmorph(logical(seg2bmap(conditionlabels(dlmread(input,' ')))),'thin',Inf);
    img = bwmorph(logical(seg2bmap(dlmread(input,' '))),'thin',Inf);
end