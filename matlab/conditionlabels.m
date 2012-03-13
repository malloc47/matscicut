function new_labels = conditionlabels(labels)

counter=0;

new_labels = labels;

while(min(min(new_labels) < 1) || counter > 50)
counter
for i = 1:size(labels,1)
    for j = 1:size(labels,2)
        if labels(i,j) == 0
            if i~=size(labels,1) && labels(i+1,j) ~= 0
                new_labels(i,j) = labels(i+1,j);
            elseif j~=size(labels,2) && labels(i,j+1) ~= 0
                new_labels(i,j) = labels(i,j+1);
            elseif i~=1 && labels(i-1,j) ~= 0
                new_labels(i,j) = labels(i-1,j);
            elseif j~=1 && labels(i,j-1) ~= 0
                new_labels(i,j) = labels(i,j-1);
            else
%                 disp('Complaining');
            end
        end
    end
end

counter = counter+1;

end
