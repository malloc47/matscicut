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

plot_data(datas{1},'fmeasure');
plot_data(datas{1},'precision');
plot_data(datas{1},'recall');

% $$$ plot_meanstd([datas datas_w],{'Proposed','Watershed'},'fmeasure');
% $$$ plot_meanstd([datas datas_w],{'Proposed','Watershed'},'precision');
% $$$ plot_meanstd([datas datas_w],{'Proposed','Watershed'},'recall');
plot_meanstd([datas datas_w],[labels labels_w],'fmeasure');
plot_meanstd([datas datas_w],[labels labels_w],'precision');
plot_meanstd([datas datas_w],[labels labels_w],'recall');

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
            if imgnum == data_out(i).skip
                continue;
            end
            label = conditionlabels(dlmread([data_out(i).path prefix ...
                                sprintf('%04d',imgnum) postfix '.' ...
                                labeltype],' '));
% $$$             edge = imdilate(bwmorph(logical(seg2bmap(label)), ...
% $$$                                     'thin',Inf),strel('disk',d));
            edge = bwmorph(logical(seg2bmap(label)),'thin',Inf);
            [fm,p,r] = score(ground,edge,d);
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

function [fm,p,r] = score(gt,t,d)
   dt = bwdist(gt);
   dt2 = bwdist(t);
   tp = sum(dt(t)<=d);
   fp = sum(dt(t)>d);
   fn = sum(dt2(gt)>d);
% $$$    fn = length(dt(t));
   p = tp/(tp+fp);
   r = tp/(tp+fn);
   fm = 2*(p*r) / (p+r);
end