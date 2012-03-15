function new_labels = conditionlabels(labels)

new_labels = labels;
[dm,dm_idx] = bwdist(labels>0);

for i=1:size(labels,1)
    for j=1:size(labels,2)
        if(labels(i,j)==0)
            new_labels(i,j)=labels(dm_idx(i,j));
        end
    end
end

end
