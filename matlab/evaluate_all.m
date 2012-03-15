function evaluate_all(imgnums)
    
source;
d=5;

disp('Creating data');

style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];

counter = 1;
for i = imgnums
    data(counter).name = [int2str(i) ' as GT'];
    data(counter).style = [style(counter) '-'];
    data(counter).path = [volume int2str(i) '/'];
    data(counter).fmeasure = [];
    data(counter).precision = [];
    data(counter).recall = [];
    data(counter).skip = i;
    data(counter).y = [];
    
    counter = counter + 1;
end

meanf = [];
stdf = [];
meanp = [];
stdp = [];
meanr = [];
stdr = [];

for imgnum = imgnums
    disp(imgnum);
    ground = logical(imread([groundpath prefix sprintf('%04d',imgnum) ...
                        postfix '.' imgtype]));
%    ground = logical(seg2bmap(dlmread([groundpath prefix sprintf('%04d',imgnum) ...
%                      postfix '.' labeltype],' ')));
    ground = bwmorph(ground,'thin',Inf);
    ground = imdilate(ground,strel('disk',d));

    vf = [];
    vp = [];
    vr = [];
    
    for i = 1:length(data)
        if imgnum == data(i).skip
            continue;
        end
        label = conditionlabels(dlmread([data(i).path prefix ...
                            sprintf('%04d',imgnum) postfix '.' ...
                            labeltype],' '));
        edge = imdilate(bwmorph(logical(seg2bmap(label)), ...
                                'thin',Inf),strel('disk',d));
        data(i).fmeasure = [data(i).fmeasure fmeasure(ground, edge)];
        data(i).precision = [data(i).precision precision(ground,edge)];
        data(i).recall = [data(i).recall recall(ground,edge)];
        data(i).y = [data(i).y imgnum];
        vf = [vf fmeasure(ground, edge)];
        vp = [vp precision(ground,edge)];
        vr = [vr recall(ground,edge)];
    end
    meanf = [meanf mean(vf)];
    stdf = [stdf std(vf)];
    meanp = [meanp mean(vp)];
    stdp = [stdp std(vp)];
    meanr = [meanr mean(vr)];
    stdr = [stdr std(vr)];
end

% save temp.mat
% load temp.mat

plot_data(data,'fmeasure');
plot_data(data,'precision');
plot_data(data,'recall');

plot_meanstd(meanf,stdf,'fmeasure');
plot_meanstd(meanp,stdp,'precision');
plot_meanstd(meanr,stdr,'recall');

end

function plot_data(data,field)
    fig = figure('visible','off'); 
    hold all;
    legend_labels = {};
    for i = 1:length(data)
        plot(data(i).y,data(i).(field),data(i).style); 
        legend_labels{i} = data(i).name;
    end
    leg = legend(legend_labels);
    set(leg,'Location','SouthEast');
    set(leg,'FontSize',12);
    xlabel('Serial Slice');
    ylabel(field);
    print('-depsc2', [field '.eps']);
end

function plot_meanstd(m,s,field)
    fig = figure('visible','off'); 
    hold all;
    eb = errorbar(1:length(m),m,s); 
    set(eb                            , ...
        'LineWidth'       , 2         , ...
        'Marker'          , 'o'       , ...
        'MarkerSize'      , 6         , ...      
        'MarkerFaceColor' , [.7 .7 .7]);
    xlabel('Serial Slice');
    ylabel(field);
    print('-depsc2', [field '-mean.eps']);
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
