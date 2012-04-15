function mean_figure_all()
source;

imgnums = 90:100;
d=3;
frame = placeholders('seq1/',imgnums);
frame_w = placeholders(['seq1/' 'watershed/'],imgnums);
frame = fill_frame(frame,imgnums,d);
frame_w = fill_frame(frame_w,imgnums,d);
data_o = single_placeholders(90,w2labelpath,n2labelpath,n1labelpath);
data_o = fill_data(data_o,imgnums,d);
titles = {'Proposed','Extended Watershed','Baseline Watershed','Extended NC','NC'};
plot_meanstd({frame,frame_w},titles, 'fmeasure',imgnums,'seq1-f.eps',data_o);
plot_meanstd({frame,frame_w},titles, 'precision',imgnums,'seq1-p.eps',data_o);
plot_meanstd({frame,frame_w},titles, 'recall',imgnums,'seq1-r.eps',data_o);

imgnums = 23:60;
d=3;
frame_w = placeholders(['seq9/' 'watershed/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d);
data_o = single_placeholders(44,'seq9/labels/');
data_o = fill_data(data_o,imgnums,d);
titles = {'Extended Watershed','Proposed'};
plot_meanstd({frame_w},titles,'fmeasure',imgnums,'seq9-f.eps',data_o);
plot_meanstd({frame_w},titles,'precision',imgnums,'seq9-p.eps',data_o);
plot_meanstd({frame_w},titles,'recall',imgnums,'seq9-r.eps',data_o);

imgnums = 40:50;
d=7;
frame_w = placeholders(['seq3/' 'watershed/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d);
frame = placeholders(['seq3/' 'skel/'],imgnums);
frame = fill_frame(frame,imgnums,d);
frame_o = placeholders(['seq3/' 'global/'],imgnums);
frame_o = fill_frame(frame_o,imgnums,d);
titles = {'Extended Watershed', 'Proposed','Proposed w/o Shape'};
plot_meanstd({frame_w,frame,frame_o},titles,'fmeasure',imgnums,'seq3-f.eps');
plot_meanstd({frame_w,frame,frame_o},titles,'precision',imgnums,'seq3-p.eps');
plot_meanstd({frame_w,frame,frame_o},titles,'recall',imgnums,'seq3-r.eps');

imgnums = 3232:3250;
d=3;
frame_w = placeholders(['seq10/' 'watershed/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d);
frame = placeholders(['seq10/' '30-0.5/'],imgnums);
frame = fill_frame(frame,imgnums,d);
% $$$ frame_o = placeholders(['seq3/' 'global/'],imgnums);
% $$$ frame_o = fill_frame(frame_o,imgnums,d);
titles = {'Extended Watershed', 'Proposed'};
plot_meanstd({frame_w,frame},titles,'fmeasure',imgnums,'seq10-f.eps');
plot_meanstd({frame_w,frame},titles,'precision',imgnums,'seq10-p.eps');
plot_meanstd({frame_w,frame},titles,'recall',imgnums,'seq10-r.eps');

imgnums = 769:780;
evalnums = imgnums;
d=7;
frame_w = placeholders(['seq12/' 'watershed-15/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d,evalnums);
%frame = placeholders(['seq12/' 'gauss-50-15-2/'],imgnums);
frame = placeholders(['seq12/' 'gauss-50-15-2/'],imgnums);
frame = fill_frame(frame,imgnums,d,evalnums);
%frame_o = placeholders(['seq12/' 'global-15/'],imgnums);
frame_o = placeholders(['seq12/' 'global-15/'],imgnums);
frame_o = fill_frame(frame_o,imgnums,d,evalnums);
data_o = single_placeholders(0,'seq12/threshold/');
data_o = fill_data(data_o,imgnums,d);
titles = {'Extended Watershed', 'Proposed','Proposed w/o Appearance','Threshold'};
plot_meanstd({frame_w,frame,frame_o},titles,'fmeasure',imgnums,'seq12-f.eps',data_o);
plot_meanstd({frame_w,frame,frame_o},titles,'precision',imgnums,'seq12-p.eps',data_o);
plot_meanstd({frame_w,frame,frame_o},titles,'recall',imgnums,'seq12-r.eps',data_o);

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

function data_o = single_placeholders(skip,varargin)
% $$$     style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];
    for i = (1:numel(varargin))
        data_o(i).style = 'o-';
        data_o(i).path = varargin{i};
        data_o(i).fmeasure = [];
        data_o(i).precision = [];
        data_o(i).recall = [];
        data_o(i).skip = skip;
        data_o(i).y = [];
    end
end

function frame_out=fill_frame(frame,imgnums,d,evalnums)
    source
    if(nargin<4)
        evalnums=imgnums;
    end
    frame_out = frame;
    for f = 1:length(frame_out)
        for imgnum = evalnums
            if frame_out(f).number == imgnum
                continue
            end
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

function plot_meanstd(frm,labels,field,imgnums,output,opt)
    fig = figure('visible','off'); 
    hold all;
    for fr = 1:length(frm)
        frames = frm{fr};
        m = [];
        s = [];
        l = {};
        for f = 1:length(frames)
            t = frames(f).(field);
            % We're plotting all possible ground truths
            if length(frames(f).(field)) == length(imgnums)
                t(frames(f).skip) = [];
            end
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
    
    if(nargin>5)
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
    print('-depsc2', output);
end
