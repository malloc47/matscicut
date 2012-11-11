function mean_figure_seq3(imgnums,d)
source;

frame_w = placeholders([volume 'watershed/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d);
frame = placeholders([volume 'skel/'],imgnums);
frame = fill_frame(frame,imgnums,d);
frame_o = placeholders([volume 'global/'],imgnums);
frame_o = fill_frame(frame_o,imgnums,d);

% $$$ data_o(1).name = 'Proposed Method';
% $$$ data_o(1).style = 'o-';
% $$$ data_o(1).path = labelpath;
% $$$ data_o(1).fmeasure = [];
% $$$ data_o(1).precision = [];
% $$$ data_o(1).recall = [];
% $$$ data_o(1).skip = 44;
% $$$ data_o(1).y = [];

% $$$ data_o = fill_data(data_o,imgnums,d);

plot_meanstd({frame_w,frame,frame_o},{'Extended Watershed', ...
                    'Proposed','Proposed w/o Shape'},'fmeasure',imgnums);
plot_meanstd({frame_w,frame,frame_o},{'Extended Watershed', ...
                    'Proposed','Proposed w/o Shape'},'precision',imgnums);
plot_meanstd({frame_w,frame,frame_o},{'Extended Watershed', ...
                    'Proposed','Proposed w/o Shape'},'recall',imgnums);

end

function frame = placeholders(path,imgnums)
    style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];
    c = 1;
    for i = imgnums
        frame(c).number = i;
        frame(c).style = [style(mod(c-1,length(style)-1)+1) '-'];
        frame(c).path = path;
        frame(c).fmeasure = [];
        frame(c).precision = [];
        frame(c).recall = [];
        frame(c).skip = c;
        c = c + 1;
    end    
end

function frame_out=fill_frame(frame,imgnums,d)
    source
    frame_out = frame;
    for f = 1:length(frame_out)
        for imgnum = imgnums
            fm = dlmread([frame_out(f).path int2str(imgnum) '/' ...
                          prefix sprintf('%04d',frame_out(f).number) ...
                          postfix '.' scoretype],',');
            frame_out(f).fmeasure = [frame_out(f).fmeasure fm(d,1)];
            frame_out(f).precision = [frame_out(f).precision fm(d,2)];
            frame_out(f).recall = [frame_out(f).recall fm(d,3)];
        end
    end
end

function data_out=fill_data(data,imgnums,d)
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
            data_out(i).fmeasure = [data_out(i).fmeasure fm(d,1)];
            data_out(i).precision = [data_out(i).precision fm(d,2)];
            data_out(i).recall = [data_out(i).recall fm(d,3)];
            data_out(i).y = [data_out(i).y imgnum];
        end
    end
end

function plot_meanstd(frm,labels,field,imgnums,opt)
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
% $$$         eb = errorbar(1:length(m),m,s); 
        eb = errorbar(imgnums,m,s); 
        set(eb                            , ...
            'LineWidth'       , 2         , ...
            'Marker'          , 'o'       , ...
            'MarkerSize'      , 6         , ...      
            'MarkerFaceColor' , [.7 .7 .7]);
    end
    
    if(nargin>4)
        for i = 1:length(opt)
            pl = plot(opt(i).y,opt(i).(field), opt(i).style);
            
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