function evaluate_all(imgnums,dists)
    
source;

if(nargin<2)
    dists=3;
end

disp('Creating data');

style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];

datas = {};
datas_w = {};

for d=dists
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
    
    datas{d} = data;
    datas_w{d} = data_w;

end

data_o(1).name = 'Watershed';
data_o(1).style = '*-';
data_o(1).path = w2labelpath;
data_o(1).fmeasure = [];
data_o(1).precision = [];
data_o(1).recall = [];
data_o(1).skip = 90;
data_o(1).y = [];

data_o(2).name = 'Normalized Cut';
data_o(2).style = '*-';
data_o(2).path = n2labelpath;
data_o(2).fmeasure = [];
data_o(2).precision = [];
data_o(2).recall = [];
data_o(2).skip = 90;
data_o(2).y = [];

data_o = fill_data(data_o,imgnums,d);

labels = {};
labels_w = {};
for d=dists
    labels{d} = ['P d=' int2str(d)];
end

for d=dists
    labels_w{d} = ['W d=' int2str(d)];
end

save temp.mat
% $$$ load temp.mat

plot_data(datas{min(dists)},'fmeasure');
plot_data(datas{min(dists)},'precision');
plot_data(datas{min(dists)},'recall');

% $$$ plot_meanstd([datas datas_w],{'Proposed','Watershed'},'fmeasure');
% $$$ plot_meanstd([datas datas_w],{'Proposed','Watershed'},'precision');
% $$$ plot_meanstd([datas datas_w],{'Proposed','Watershed'},'recall');
plot_meanstd([datas datas_w],[labels labels_w],'fmeasure',imgnums);
plot_meanstd([datas datas_w],[labels labels_w],'precision',imgnums);
plot_meanstd([datas datas_w],[labels labels_w],'recall',imgnums);

end

function data_out=fill_data(data,imgnums,d)
    source
    data_out = data;
    for imgnum = imgnums
        disp(imgnum);
        ground = logical(imread([groundpath prefix sprintf('%04d',imgnum) ...
                            postfix '.' imgtype]));
        ground = bwmorph(ground,'thin',Inf);
% $$$         ground = imdilate(ground,strel('disk',d));

        for i = 1:length(data_out)
            if ismember(imgnum,data_out(i).skip)
                % pad in zeros
% $$$                 data_out(i).fmeasure = [data_out(i).fmeasure 0];
% $$$                 data_out(i).precision = [data_out(i).precision 0];
% $$$                 data_out(i).recall = [data_out(i).recall 0];
% $$$                 data_out(i).y = [data_out(i).y imgnum];
                continue;
            end
            label = conditionlabels(dlmread([data_out(i).path prefix ...
                                sprintf('%04d',imgnum) postfix '.' ...
                                labeltype],' '));
% $$$             edge = imdilate(bwmorph(logical(seg2bmap(label)), ...
% $$$                                     'thin',Inf),strel('disk',d));
            edge = bwmorph(logical(seg2bmap(label)),'thin',Inf);
            [fm,p,r] = fmeasure(ground,edge,d);
% $$$             data_out(i).fmeasure = [data_out(i).fmeasure fmeasure(ground, edge)];
% $$$             data_out(i).precision = [data_out(i).precision precision(ground,edge)];
% $$$             data_out(i).recall = [data_out(i).recall recall(ground,edge)];
            data_out(i).fmeasure = [data_out(i).fmeasure fm];
            data_out(i).precision = [data_out(i).precision p];
            data_out(i).recall = [data_out(i).recall r];
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

function plot_meanstd(datas,labels,field,imgnums,opt)
    fig = figure('visible','off'); 
    hold all;
    
    for d = find(~cellfun('isempty',datas))
        m = [];
        s = [];
        for i = 1:length(datas{d}(1).fmeasure) % imgnums %  1:length(datas{d}(1).fmeasure)
            tmp_score = [];
            for j = 1:length(datas{d}(i).(field)) % 1:length(datas{d}(i).(field)))
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
    
    c=1;
    new_labels={};
    for l=find(~cellfun('isempty',datas))
        new_labels{c} = labels{l};
        c=c+1;
    end
    
    if(nargin>4)
        for i = 1:length(opt)
% $$$             plot(opt(i).y,opt(i).(field),opt(i).style); 
            plot(1:length(opt(i).(field)),opt(i).(field),opt(i).style); 
            new_labels{c} = opt(i).name;
            c=c+1;
        end
    end
    
    leg = legend(new_labels);
    set(leg,'Location','SouthEast');
    set(leg,'FontSize',12);
    xlabel('Serial Slice');
    ylabel(field);
    print('-depsc2', [field '-mean.eps']);
end