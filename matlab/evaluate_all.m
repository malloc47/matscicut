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

counter = 1;
for i = imgnums
    data_w(counter).name = [int2str(i) ' as GT'];
    data_w(counter).style = [style(counter) '-'];
    data_w(counter).path = [volume 'watershed/' int2str(i) '/'];
    data_w(counter).fmeasure = [];
    data_w(counter).precision = [];
    data_w(counter).recall = [];
    data_w(counter).skip = i;
    data_w(counter).y = [];
    
    counter = counter + 1;
end

data = fill_data(data,imgnums,d);
data_w = fill_data(data_w,imgnums,d);

save temp.mat
% $$$ load temp.mat

plot_data(data,'fmeasure');
plot_data(data,'precision');
plot_data(data,'recall');

plot_meanstd({data,data_w},{'Proposed','Watershed'},'fmeasure');
plot_meanstd({data,data_w},{'Proposed','Watershed'},'precision');
plot_meanstd({data,data_w},{'Proposed','Watershed'},'recall');

end

function data_out=fill_data(data,imgnums,d)
    source
    data_out = data;
    for imgnum = imgnums
        disp(imgnum);
        ground = logical(imread([groundpath prefix sprintf('%04d',imgnum) ...
                            postfix '.' imgtype]));
        ground = bwmorph(ground,'thin',Inf);
        ground = imdilate(ground,strel('disk',d));

        for i = 1:length(data_out)
            if imgnum == data_out(i).skip
                continue;
            end
            label = conditionlabels(dlmread([data_out(i).path prefix ...
                                sprintf('%04d',imgnum) postfix '.' ...
                                labeltype],' '));
            edge = imdilate(bwmorph(logical(seg2bmap(label)), ...
                                    'thin',Inf),strel('disk',d));
            data_out(i).fmeasure = [data_out(i).fmeasure fmeasure(ground, edge)];
            data_out(i).precision = [data_out(i).precision precision(ground,edge)];
            data_out(i).recall = [data_out(i).recall recall(ground,edge)];
            data_out(i).y = [data_out(i).y imgnum];
        end
    end
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

function plot_meanstd(datas,labels,field)
    fig = figure('visible','off'); 
    hold all;
    
    for d = 1:length(datas)
        m = [];
        s = [];
        for i = 1:length(datas{d}(1).fmeasure)
            tmp_score = [];
            for j = 1:length(datas{d})
                tmp_score = [tmp_score datas{d}(j).(field)(i)];
            end
            m = [m mean(tmp_score)];
            s = [s std(tmp_score)];
        end
        
        eb = errorbar(1:length(m),m,s); 
        set(eb                            , ...
            'LineWidth'       , 2         , ...
            'Marker'          , 'o'       , ...
            'MarkerSize'      , 6         , ...      
            'MarkerFaceColor' , [.7 .7 .7]);
    end
    leg = legend(labels);
    set(leg,'Location','SouthEast');
    set(leg,'FontSize',12);
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
