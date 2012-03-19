function mean_figure(imgnums)
source;

frame = placeholders(volume,imgnums);
frame_w = placeholders([volume 'watershed/'],imgnums);

frame = fill_frame(frame,imgnums);
frame_w = fill_frame(frame_w,imgnums);

data_o(1).name = 'Baseline Watershed';
data_o(1).style = 'o-';
data_o(1).path = w2labelpath;
data_o(1).fmeasure = [];
data_o(1).precision = [];
data_o(1).recall = [];
data_o(1).skip = 90;
data_o(1).y = [];

data_o(2).name = 'Extended Normalized Cut';
data_o(2).style = 'o-';
data_o(2).path = n2labelpath;
data_o(2).fmeasure = [];
data_o(2).precision = [];
data_o(2).recall = [];
data_o(2).skip = 90;
data_o(2).y = [];

data_o(3).name = 'Normalized Cut';
data_o(3).style = 'o-';
data_o(3).path = n1labelpath;
data_o(3).fmeasure = [];
data_o(3).precision = [];
data_o(3).recall = [];
data_o(3).skip = 90;
data_o(3).y = [];

data_o = fill_data(data_o,imgnums);

plot_meanstd({frame,frame_w},{'Proposed','Extended Watershed',['Baseline ' ...
                    '' 'Watershed'],'Extended NC','NC'},'fmeasure',data_o);

plot_meanstd({frame,frame_w},{'Proposed','Extended Watershed',['Baseline ' ...
                    '' 'Watershed'],'Extended NC','NC'},'precision',data_o);

plot_meanstd({frame,frame_w},{'Proposed','Extended Watershed',['Baseline ' ...
                    '' 'Watershed'],'Extended NC','NC'},'recall',data_o);

end

function frame = placeholders(path,imgnums)
    style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];
    c = 1;
    for i = imgnums
        frame(c).number = i;
        frame(c).style = [style(c) '-'];
        frame(c).path = path;
        frame(c).fmeasure = [];
        frame(c).precision = [];
        frame(c).recall = [];
        frame(c).skip = c;
        c = c + 1;
    end    
end

function frame_out=fill_frame(frame,imgnums)
    source
    frame_out = frame;
    for f = 1:length(frame_out)
        for imgnum = imgnums
            fm = dlmread([frame_out(f).path int2str(imgnum) '/' ...
                          prefix sprintf('%04d',frame_out(f).number) ...
                          postfix '.' scoretype],',');
            frame_out(f).fmeasure = [frame_out(f).fmeasure fm(1)];
            frame_out(f).precision = [frame_out(f).precision fm(2)];
            frame_out(f).recall = [frame_out(f).recall fm(3)];
        end
    end
end

function data_out=fill_data(data,imgnums)
    source
    data_out = data;
    for imgnum = imgnums
        disp(imgnum);
        for i = 1:length(data_out)
            if ismember(imgnum,data_out(i).skip)
                continue;
            end
            fm = dlmread([data_out(i).path prefix sprintf('%04d',imgnum) ...
                          postfix '.' scoretype],',');
            data_out(i).fmeasure = [data_out(i).fmeasure fm(1)];
            data_out(i).precision = [data_out(i).precision fm(2)];
            data_out(i).recall = [data_out(i).recall fm(3)];
            data_out(i).y = [data_out(i).y imgnum];
        end
    end
end

function plot_meanstd(frm,labels,field,opt)
    fig = figure('visible','off'); 
    hold all;
    for fr = 1:length(frm)
        frames = frm{fr};
        m = [];
        s = [];
        l = {};
        for f = 1:length(frames)
            t = frames(f).(field);
            t(frames(f).skip) = [];
            m = [m mean(t)];
            s = [s std(t)];
            l{f} = int2str(frames(f).number);
        end
        
        eb = errorbar(1:length(m),m,s); 
        set(eb                            , ...
            'LineWidth'       , 2         , ...
            'Marker'          , 'o'       , ...
            'MarkerSize'      , 6         , ...      
            'MarkerFaceColor' , [.7 .7 .7]);
    end
    
    if(nargin>3)
% $$$         c=length(labels
        for i = 1:length(opt)
            pl = plot(1:length(opt(i).(field)),opt(i).(field), ...
                      opt(i).style);
            
            set(pl, ...
                'LineWidth'       , 2         , ...
                'Marker'          , 'o'       , ...
                'MarkerSize'      , 6         , ...      
                'MarkerFaceColor' , [.7 .7 .7]);
            
        end
    end
    
    leg = legend(labels);
    set(leg,'Location','SouthEast');
    set(leg,'FontSize',12);
    xlabel('Serial Slice');
    ylabel(field);
    print('-depsc2', [field '-mean.eps']);
end