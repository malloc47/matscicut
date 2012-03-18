function evaluate_file(ground_file,file,outfile,d)

    if(nargin < 4)
        d=3;
    end
    
    [f,p,r] = fmeasure(read_file(ground_file),read_file(file),d);
    
    dlmwrite(outfile,[f,p,r],',');
end

function img = read_file(input)
    img = bwmorph(logical(seg2bmap(conditionlabels(dlmread(input,' ')))),'thin',Inf);
end