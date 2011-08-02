function evaluate3(imgnums)
    
source;
d=4;

disp('Creating data');

% data = struct( 'name', {'Proposed Method','Extended Watershed', ...
%                     'Baseline Watershed' ,'Extended Normalized Cut', ...
%                     'Baseline Normalized Cut'}, ...
%                'style', {'r.-','m*-','g+-','b*-','cx-'}, ...
%                'path', {labelpath,w1labelpath,w2labelpath, ...
%                     n1labelpath,n2labelpath}, ...
%                'fmeasure', {{},{},{},{},{}}, ...
%                'precision', {0,0,0,0,0}, ...
%                'recall', {0,0,0,0,0});

data(1).name = 'Proposed Method';
data(1).style = 'r.-';
data(1).path = labelpath;
data(1).fmeasure = [];
data(1).precision = [];
data(1).recall = [];

data(2).name = 'Extended Watershed';
data(2).style = 'm*-';
data(2).path = w1labelpath;
data(2).fmeasure = [];
data(2).precision = [];
data(2).recall = [];

data(3).name = 'Extended Normalized Cut';
data(3).style = 'b*-';
data(3).path = n1labelpath;
data(3).fmeasure = [];
data(3).precision = [];
data(3).recall = [];


for imgnum = imgnums
    disp(imgnum);
    
    ground = logical(seg2bmap(dlmread([groundpath prefix sprintf('%04d',imgnum) ...
                      postfix '.' labeltype],' ')));
    ground = bwmorph(ground,'thin',Inf);
    ground = imdilate(ground,strel('disk',d));

    for i = 1:length(data)
        label = conditionlabels(dlmread([data(i).path prefix ...
                            sprintf('%04d',imgnum) postfix '.' ...
                            labeltype],' '));
        edge = imdilate(bwmorph(logical(seg2bmap(label)), ...
                                'thin',Inf),strel('disk',d));
        data(i).fmeasure = [data(i).fmeasure fmeasure(ground, edge)];
        data(i).precision = [data(i).precision precision(ground,edge)];
        data(i).recall = [data(i).recall recall(ground,edge)];
    end
end

save temp.mat
% load temp.mat

plot_data(data,'fmeasure');
plot_data(data,'precision');
plot_data(data,'recall');

end

function plot_data(data,field)
    fig = figure('visible','off'); 
    hold all;
    legend_labels = {};
    for i = 1:length(data)
        yax = 2:length(data(i).(field))+1;
        plot(yax,data(i).(field),data(i).style); 
        legend_labels{i} = data(i).name;
    end
    leg = legend(legend_labels);
    set(leg,'Location','SouthEast');
    set(leg,'FontSize',12);
    xlabel('Serial Slice');
    ylabel(field);
    print('-depsc2', [field '.eps']);
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
	d=2;
	%num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',5))) ));
	%num = sum(sum( bwmorph(gt,'thin',Inf) & ~(imdilate(ev,strel('disk',d))) ));
	num = sum(sum( gt & (~ev) ));
end
