function mean_figure_all()
source;

%% Seq1: Ti-26

imgnums = 90:100;
d=3;
frame = placeholders('seq1/',imgnums);
frame_w = placeholders(['seq1/' 'watershed/'],imgnums);
frame = fill_frame(frame,imgnums,d);
frame_w = fill_frame(frame_w,imgnums,d);
data_o = single_placeholders(90,w2labelpath,n2labelpath);
data_o = fill_data(data_o,imgnums,d);
titles = {'Proposed','Propagated Watershed','Baseline Watershed','Intensity-based NC'};
fstart(); plot_meanstd({frame,frame_w},'fmeasure',imgnums,data_o); fend(titles,'fig/seq1-f.eps','fmeasure');
fstart(); plot_meanstd({frame,frame_w},'precision', imgnums,data_o); fend(titles,'fig/seq1-p.eps','precision');
fstart(); plot_meanstd({frame,frame_w},'recall',imgnums,data_o); fend(titles,'fig/seq1-r.eps','recall');
fstart(); plot_meanstd({frame,frame_w},'numdiff',imgnums,data_o); fend(titles,'fig/seq1-n.eps','numdiff');

data_s = single_placeholders([90,95,100,90,95,100],['seq1/90/'],['seq1/95/'],['seq1/100/'],['seq1/watershed/90/'],['seq1/watershed/95/'],['seq1/watershed/100/']);
data_s = fill_data(data_s,imgnums,d);
fstart(); 
plot_single_curve(data_s,'fmeasure'); 
fend({'90 Proposed','95 Proposed','100 Proposed','90 Watershed','95 Watershed','100 Watershed'},'fig/seq1-s.eps','fmeasure');

%% Seq9: Cotylendon

imgnums = 23:60;
d=3;
frame_w = placeholders(['seq9/' 'watershed/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d);
% $$$ data_o = single_placeholders(44,'seq9/labels/','seq9/nc/labels/');
data_o = single_placeholders(44,'seq9/44/');
data_o = fill_data(data_o,imgnums,d);
% $$$ titles = {'Propagated Watershed','Proposed','NC'};
titles = {'Propagated Watershed','Proposed'};
fstart(); plot_meanstd({frame_w},'fmeasure',imgnums,data_o); fend(titles,'fig/seq9-f.eps','fmeasure');
fstart(); plot_meanstd({frame_w},'precision',imgnums,data_o); fend(titles,'fig/seq9-p.eps','precision');
fstart(); plot_meanstd({frame_w},'recall',imgnums,data_o); fend(titles,'fig/seq9-r.eps','recall');
fstart(); plot_meanstd({frame_w},'numdiff',imgnums,data_o); fend(titles,'fig/seq9-n.eps','numdiff');

data_s = single_placeholders([44,38,44,50],['seq9/44/'],['seq9/watershed/38/'],['seq9/watershed/44/'],['seq9/watershed/50/']);
data_s = fill_data(data_s,imgnums,d);
fstart(); 
plot_single_curve(data_s,'fmeasure'); 
fend({'44 Proposed','38 Watershed','44 Watershed','50 Watershed'},'fig/seq9-s.eps','fmeasure');

%% Seq3: Steel Martensite

imgnums = 40:50;
d=7;
frame_w = placeholders(['seq3/' 'watershed/'],imgnums);
frame_w = fill_frame(frame_w,imgnums,d);
frame = placeholders(['seq3/' 'skel/'],imgnums);
frame = fill_frame(frame,imgnums,d);
frame_o = placeholders(['seq3/' 'global/'],imgnums);
frame_o = fill_frame(frame_o,imgnums,d);
titles = {'Propagated Watershed', 'Proposed','Proposed w/o Shape'};
fstart(); plot_meanstd({frame_w,frame,frame_o},'fmeasure',imgnums); fend(titles,'fig/seq3-f.eps','fmeasure');
fstart(); plot_meanstd({frame_w,frame,frame_o},'precision',imgnums); fend(titles,'fig/seq3-p.eps','precision');
fstart(); plot_meanstd({frame_w,frame,frame_o},'recall',imgnums); fend(titles,'fig/seq3-r.eps','recall');
fstart(); plot_meanstd({frame_w,frame,frame_o},'numdiff',imgnums); fend(titles,'fig/seq3-n.eps','numdiff');

data_s = single_placeholders([40,45,50,40,45,50],['seq3/skel/40/'],['seq3/skel/45/'],['seq3/skel/50/'],['seq3/watershed/40/'],['seq3/watershed/45/'],['seq3/watershed/50/']);
data_s = fill_data(data_s,imgnums,d);
fstart(); 
plot_single_curve(data_s,'fmeasure'); 
fend({'40 Proposed','45 Proposed','50 Proposed','40 Watershed','45 Watershed','50 Watershed'},'fig/seq3-s.eps','fmeasure');

%% Seq10: Rat Brain

% $$$ imgnums = 3232:3250;
% $$$ d=3;
% $$$ frame_w = placeholders(['seq10/' 'watershed/'],imgnums);
% $$$ frame_w = fill_frame(frame_w,imgnums,d);
% $$$ frame = placeholders(['seq10/' '30-0.5/'],imgnums);
% $$$ frame = fill_frame(frame,imgnums,d);
% $$$ % $$$ frame_o = placeholders(['seq3/' 'global/'],imgnums);
% $$$ % $$$ frame_o = fill_frame(frame_o,imgnums,d);
% $$$ titles = {'Propagated Watershed', 'Proposed'};
% $$$ fstart(); plot_meanstd({frame_w,frame},'fmeasure',imgnums); fend(titles,'fig/seq10-f.eps','fmeasure');
% $$$ fstart(); plot_meanstd({frame_w,frame},'precision',imgnums); fend(titles,'fig/seq10-p.eps','precision');
% $$$ fstart(); plot_meanstd({frame_w,frame},'recall',imgnums); fend(titles,'fig/seq10-r.eps','recall');
% $$$ % $$$ fstart(); plot_meanstd({frame_w,frame},'numdiff',imgnums); fend(titles,'seq10-n.eps','numdiff');

%% Seq12: Epoxy

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
data_o = single_placeholders(0,'seq12/threshold/','seq12/nc/labels/');
data_o = fill_data(data_o,imgnums,d);
titles = {'Propagated Watershed', 'Proposed','Proposed w/o Appearance','Threshold','NC'};
fstart(); plot_meanstd({frame_w,frame,frame_o},'fmeasure',imgnums,data_o); fend(titles,'fig/seq12-f.eps','fmeasure');
fstart(); plot_meanstd({frame_w,frame,frame_o},'precision',imgnums,data_o); fend(titles,'fig/seq12-p.eps','precision');
fstart(); plot_meanstd({frame_w,frame,frame_o},'recall',imgnums,data_o); fend(titles,'fig/seq12-r.eps','recall');
fstart(); plot_meanstd({frame_w,frame,frame_o},'numdiff',imgnums,data_o); fend(titles,'fig/seq12-n.eps','numdiff');

data_s = single_placeholders([769,775,780,769,775,780],['seq12/gauss-50-15-2/769/'],['seq12/gauss-50-15-2/775/'],['seq12/gauss-50-15-2/780/'],['seq12/watershed-15/769/'],['seq12/watershed-15/775/'],['seq12/watershed-15/780/']);
data_s = fill_data(data_s,imgnums,d);
fstart(); 
plot_single_curve(data_s,'fmeasure'); 
fend({'769 Proposed','775 Proposed','780 Proposed','769 Watershed','775 Watershed','780 Watershed'},'fig/seq12-s.eps','fmeasure');

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
        frame(c).numdiff = [];
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
        data_o(i).numdiff = [];
        data_o(i).skip = skip(mod(i-1,length(skip))+1);
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
            if(size(fm,2) > 3)
                frame_out(f).numdiff = [frame_out(f).numdiff fm(d,4)];
            end
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
            if(size(fm,2) > 3)
                data_out(i).numdiff = [data_out(i).numdiff fm(d,4)];
            end
            data_out(i).y = [data_out(i).y imgnum];
        end
    end
end

function plot_single_curve(l,field,colors)
    for i = 1:length(l)
        style = l(i).style;
        if nargin > 2
            style = [style colors(mod(i-1,length(colors))+1) ];
        end
        pl = plot(l(i).y,l(i).(field), style);
        set(pl, ...
            'LineWidth'       , 2         , ...
            'MarkerSize'      , 6         , ...      
            'MarkerFaceColor' , [.7 .7 .7]);     
    end    
end

function plot_meanstd(frm,field,imgnums,opt)
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
    
    if(nargin>3)
        plot_single_curve(opt,field);
    end
end

function fstart()
    fig = figure('visible','off'); 
    hold all;
end

function fend(labels,output,field)
    leg = legend(labels);
    set(leg,'Location','SouthEast');
    set(leg,'FontSize',12);
    xlabel('Serial Slice');
    ylabel(field);
    print('-depsc2', output);
end