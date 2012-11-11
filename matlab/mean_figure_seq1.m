function mean_figure_seq1()
%% Boilerplate
source;
p_color  = [0 0 1];
p2_color = [0 0 0.5];
p3_color = [0 1 1];
w_color  = [0 0.5 0];
w2_color = [0 1 0];
w3_color = [0.5 1 0.5];
n_color  = [1 0 1];
t_color  = [1 1 0];
cl_color  = [1 1 0];
cl2_color  = [0.5 0.5 0];

%% Seq1: Ti-26

imgnums = 90:100;
subtr = 89;
d=3;
frame = placeholders('seq1/global-20/',imgnums);
frame_c = placeholders('seq1/global-no-local-20/',imgnums);
frame_w = placeholders('seq1/watershed/',imgnums);
frame_cl = placeholders('seq1/clique-local/',imgnums);
frame_cl2 = placeholders('seq1/clique-local-2/',imgnums);
frame = fill_frame(frame,imgnums,d);
frame(1).color = p_color;
frame_c = fill_frame(frame_c,imgnums,d);
frame_c(1).color = p2_color;
frame_w = fill_frame(frame_w,imgnums,d);
frame_w(1).color = w_color;

frame_cl = fill_frame(frame_cl,imgnums,d);
%frame_cl(1).color = cl_color;
frame_cl(1).color = 'r';
frame_cl2 = fill_frame(frame_cl2,imgnums,d);
frame_cl2(1).color = cl2_color;

%data_o = single_placeholders(90,w2labelpath,n2labelpath);
%data_o = fill_data(data_o,imgnums,d);
%data_o(1).color = w2_color;
%data_o(2).color = n_color;
%data_o(1).y = data_o(1).y - subtr ;
%data_o(2).y = data_o(2).y - subtr;
data_o = single_placeholders(90,n2labelpath);
data_o = fill_data(data_o,imgnums,d);
data_o(1).color = n_color;
data_o(1).y = data_o(1).y - subtr;
%titles = {'Proposed Local+Local','Previous Global+Local','Previous Global','Propagated Watershed','Unpropagated Watershed','Normalized Cut'};
%titles = {'Proposed Local+Local','Previous Global+Local','Previous Global','Propagated Watershed','Normalized Cut'};
titles = {'Proposed+Relabeling','Global+Relabeling','Propagated Watershed','Normalized Cut'};
%fstart(); plot_meanstd({frame_cl2, frame_cl,frame,frame_c,frame_w},'fmeasure',imgnums-subtr,data_o); fend(titles,'fig/seq1-f.eps','fmeasure');

fstart(); plot_meanstd({frame_cl,frame,frame_w},'fmeasure',imgnums-subtr,data_o); fend(titles,'fig/seq1-f.eps','fmeasure');

fstart(); plot_meanstd({frame_cl,frame,frame_w},'precision', imgnums-subtr,data_o); fend(titles,'fig/seq1-p.eps','precision');
fstart(); plot_meanstd({frame_cl,frame,frame_w},'recall',imgnums-subtr,data_o); fend(titles,'fig/seq1-r.eps','recall');
fstart(); plot_meanstd({frame_cl,frame,frame_w},'numdiff',imgnums-subtr,data_o); fend_upper(titles,'fig/seq1-n.eps','numdiff');
%fstart(); plot_meanstd({frame_cl2, frame_cl,frame,frame_c},'fmeasure',imgnums-subtr,data_o); fend(titles,'fig/seq1-f.eps','fmeasure');

%fstart(); plot_meanstd({frame_cl2, frame_cl,frame,frame_c,frame_w},'precision', imgnums-subtr,data_o); fend(titles,'fig/seq1-p.eps','precision');
%fstart(); plot_meanstd({frame_cl2, frame_cl,frame,frame_c,frame_w},'recall',imgnums-subtr,data_o); fend(titles,'fig/seq1-r.eps','recall');
%fstart(); plot_meanstd({frame_cl2, frame_cl,frame,frame_c,frame_w},'numdiff',imgnums-subtr,data_o); fend(titles,'fig/seq1-n.eps','numdiff');
data_s = single_placeholders([90,95,100,90,95,100] , ...
                             ['seq1/global-20/90/'] , ...
                             ['seq1/global-20/95/'] , ...
                             ['seq1/global-20/100/'], ...
                             ['seq1/watershed/90/'] , ...
                             ['seq1/watershed/95/'] , ...
                             ['seq1/watershed/100/'], ...
                             ['seq1/nc/labels/']);
data_s = fill_data(data_s,imgnums,d);
data_s = set_field(data_s,'color', ...
                          [ p_color; ...
                    p2_color; ...
                    p3_color; ...
                    w_color ; ... 
                    w2_color; ...
                    w3_color; ...
                    n_color]);
data_s(1).y = data_s(1).y-subtr;
data_s(2).y = data_s(2).y-subtr;
data_s(3).y = data_s(3).y-subtr;
data_s(4).y = data_s(4).y-subtr;
data_s(5).y = data_s(5).y-subtr;
data_s(6).y = data_s(6).y-subtr;
data_s(7).y = data_s(7).y-subtr;
fstart(); 
plot_single_curve(data_s,'fmeasure'); 
fend({'90 Proposed'  , ...
      '95 Proposed'  , ...
      '100 Proposed' , ...
      '90 Watershed' , ...
      '95 Watershed' , ...
      '100 Watershed', ...
      'Edge-based NC'}, ...
     'fig/seq1-s.eps','fmeasure');

end

function frame = placeholders(path,imgnums)
    style = ['o','*','d','s','+','.','x','^','v','>','<','p','h'];
    c = 1;
    for i = imgnums
        frame(c).number = i;
        frame(c).style = [style(mod(c-1,length(style)-1)+1) '-'];
        frame(c).color = [];
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
        data_o(i).color = [];
        data_o(i).path = varargin{i};
        data_o(i).fmeasure = [];
        data_o(i).precision = [];
        data_o(i).recall = [];
        data_o(i).numdiff = [];
        data_o(i).skip = skip(mod(i-1,length(skip))+1);
        data_o(i).y = [];
    end
end

function frame_out = set_field(frames,field,values)
    frame_out = frames;
    s = size(values);
    for f = 1:length(frame_out)
        if(s(1)==1 || s(2)==1)
            frame_out(f).(field) = values(mod(f-1,size(values,2))+1);
        elseif(~iscell(values))
% $$$             values(mod(f-1,size(values,1))+1,:)
            frame_out(f).(field) = values(mod(f-1,size(values,1))+1,:);
        else
            frame_out(f).(field) = values{mod(f-1,length(values))+1};
        end
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
% $$$         if nargin > 2
% $$$             style = [style colors(mod(i-1,length(colors))+1) ];
% $$$         end
        pl = plot(l(i).y,l(i).(field), style);
        set(pl, ...
            'LineWidth'       , 2         , ...
            'MarkerSize'      , 6         , ...      
            'MarkerFaceColor' , [.7 .7 .7]);
        if nargin > 2
            set(pl,'Color',colors(mod(i-1,length(colors))+1,:));
        elseif(~isempty(l(i).color))
            set(pl,'Color',l(i).color);
        end
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
        if(~isempty(frames(1).color))
            set(eb,'Color',frames(1).color);
        end
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
    set(leg,'FontSize',20);
    xlabel('Serial Slice');
    ylabel(field);
	set(findobj('type','axes'),'fontsize',18) 
    print('-depsc2', output);
end

function fend_upper(labels,output,field)
    leg = legend(labels);
    set(leg,'Location','NorthEast');
    set(leg,'FontSize',20);
    xlabel('Serial Slice');
    ylabel(field);
	set(findobj('type','axes'),'fontsize',18) 
    print('-depsc2', output);
end
