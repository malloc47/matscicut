function syn_fig_all()
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
%cl_color  = [1 1 0];
%cl2_color  = [0.5 0.5 0];
cl_color  = [1 0 0];
cl2_color  = [0.5 0 0];

%% Synthetic evaluation

imgnums = 0:49;
d=2;
frame1 = placeholders('syn/d1s2/global-5/',imgnums);
frame2 = placeholders('syn/d1s2/local-5/',imgnums);
frame3 = placeholders('syn/d1s2/global-local-5/',imgnums);
frame4 = placeholders('syn/d1s2/local-local-5/',imgnums);

frame1 = fill_frame(frame1,imgnums,d,[5,15,25,35,45]);
frame1(1).color = p2_color;
frame2 = fill_frame(frame2,imgnums,d,[5,15,25,35,45]);
frame2(1).color = cl2_color;
frame3 = fill_frame(frame3,imgnums,d,[5,15,25,35,45]);
frame3(1).color = p_color;
frame4 = fill_frame(frame4,imgnums,d,[5,15,25,35,45]);
frame4(1).color = cl_color;

titles = {'Global','Local','Global+Local','Local+Local'};
fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'fmeasure',imgnums); fend(titles,'fig/d1s2-f.eps','fmeasure');

fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'precision', imgnums); fend(titles,'fig/d1s2-p.eps','precision');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'recall',imgnums); fend(titles,'fig/d1s2-r.eps','recall');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'numdiff',imgnums); fend(titles,'fig/d1s2-n.eps','numdiff');

imgnums = 0:49;
d=2;
frame1 = placeholders('syn/d1s3/global-5/',imgnums);
frame2 = placeholders('syn/d1s3/local-5/',imgnums);
frame3 = placeholders('syn/d1s3/global-local-5/',imgnums);
frame4 = placeholders('syn/d1s3/local-local-5/',imgnums);

frame1 = fill_frame(frame1,imgnums,d,[5,15,25,35,45]);
frame1(1).color = p2_color;
frame2 = fill_frame(frame2,imgnums,d,[5,15,25,35,45]);
frame2(1).color = cl2_color;
frame3 = fill_frame(frame3,imgnums,d,[5,15,25,35,45]);
frame3(1).color = p_color;
frame4 = fill_frame(frame4,imgnums,d,[5,15,25,35,45]);
frame4(1).color = cl_color;

titles = {'Global','Local','Global+Local','Local+Local'};
fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'fmeasure',imgnums); fend(titles,'fig/d1s3-f.eps','fmeasure');

fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'precision', imgnums); fend(titles,'fig/d1s3-p.eps','precision');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'recall',imgnums); fend(titles,'fig/d1s3-r.eps','recall');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4},'numdiff',imgnums); fend(titles,'fig/d1s3-n.eps','numdiff');


imgnums = 0:49;
d=2;
frame1 = placeholders('syn/d1s10/global-10/',imgnums);
frame2 = placeholders('syn/d1s10/local-10/',imgnums);
frame3 = placeholders('syn/d1s10/global-local-10/',imgnums);
frame4 = placeholders('syn/d1s10/local-local-10/',imgnums);
frame5 = placeholders('syn/d1s10/watershed-10/',imgnums);

frame1 = fill_frame(frame1,imgnums,d,[5,15,25,35,45]);
frame1(1).color = p2_color;
frame2 = fill_frame(frame2,imgnums,d,[5,15,25,35,45]);
frame2(1).color = cl2_color;
frame3 = fill_frame(frame3,imgnums,d,[5,15,25,35,45]);
frame3(1).color = p_color;
frame4 = fill_frame(frame4,imgnums,d,[5,15,25,35,45]);
frame4(1).color = cl_color;
frame5 = fill_frame(frame5,imgnums,d,[5,15,25,35,45]);
frame5(1).color = w_color;

titles = {'Global','Local','Global+Local','Local+Local','Watershed'};
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'fmeasure',imgnums); fend(titles,'fig/d1s10-f.eps','fmeasure');

fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'precision', imgnums); fend(titles,'fig/d1s10-p.eps','precision');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'recall',imgnums); fend(titles,'fig/d1s10-r.eps','recall');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'numdiff',imgnums); fend(titles,'fig/d1s10-n.eps','numdiff');

imgnums = 0:49;
d=2;
frame1 = placeholders('syn/d1s11/global-10/',imgnums);
frame2 = placeholders('syn/d1s11/local-10/',imgnums);
frame3 = placeholders('syn/d1s11/global-local-10/',imgnums);
frame4 = placeholders('syn/d1s11/local-local-10/',imgnums);
frame5 = placeholders('syn/d1s11/watershed-10/',imgnums);

frame1 = fill_frame(frame1,imgnums,d,[5,15,25,35,45]);
frame1(1).color = p2_color;
frame2 = fill_frame(frame2,imgnums,d,[5,15,25,35,45]);
frame2(1).color = cl2_color;
frame3 = fill_frame(frame3,imgnums,d,[5,15,25,35,45]);
frame3(1).color = p_color;
frame4 = fill_frame(frame4,imgnums,d,[5,15,25,35,45]);
frame4(1).color = cl_color;
frame5 = fill_frame(frame5,imgnums,d,[5,15,25,35,45]);
frame5(1).color = w_color;

titles = {'Global','Local','Global+Local','Local+Local','Watershed'};
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'fmeasure',imgnums); fend(titles,'fig/d1s11-f.eps','fmeasure');

fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'precision', imgnums); fend(titles,'fig/d1s11-p.eps','precision');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'recall',imgnums); fend(titles,'fig/d1s11-r.eps','recall');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'numdiff',imgnums); fend(titles,'fig/d1s11-n.eps','numdiff');

imgnums = 0:49;
d=2;
frame1 = placeholders('syn/d1s12/global-10/',imgnums);
frame2 = placeholders('syn/d1s12/local-10/',imgnums);
frame3 = placeholders('syn/d1s12/global-local-10/',imgnums);
frame4 = placeholders('syn/d1s12/local-local-10/',imgnums);
frame5 = placeholders('syn/d1s12/watershed-10/',imgnums);

frame1 = fill_frame(frame1,imgnums,d,[5,15,25,35,45]);
frame1(1).color = p2_color;
frame2 = fill_frame(frame2,imgnums,d,[5,15,25,35,45]);
frame2(1).color = cl2_color;
frame3 = fill_frame(frame3,imgnums,d,[5,15,25,35,45]);
frame3(1).color = p_color;
frame4 = fill_frame(frame4,imgnums,d,[5,15,25,35,45]);
frame4(1).color = cl_color;
frame5 = fill_frame(frame5,imgnums,d,[5,15,25,35,45]);
frame5(1).color = w_color;

titles = {'Global','Local','Global+Local','Local+Local','Watershed'};
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'fmeasure',imgnums); fend(titles,'fig/d1s12-f.eps','fmeasure');

fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'precision', imgnums); fend(titles,'fig/d1s12-p.eps','precision');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'recall',imgnums); fend(titles,'fig/d1s12-r.eps','recall');
fstart(); plot_meanstd({frame1, frame2, frame3, frame4, frame5},'numdiff',imgnums); fend(titles,'fig/d1s12-n.eps','numdiff');

imgnums = 15:240
d=3
data_s = single_placeholders([150,150,150,150] , ...
                             ['syn/d1s16/global-10/150/'] , ...
                             ['syn/d1s16/local-10/150/'] , ...
                             ['syn/d1s16/global-local-10/150/'], ...
                             ['syn/d1s16/local-local-10/150/']);
data_s = fill_data(data_s,imgnums,d);
data_s = set_field(data_s,'color', ...
                          [ p_color; ...
                    w_color; ...
                    p2_color; ...
                    w2_color]);
fstart(); 
plot_single_curve(data_s,'fmeasure'); 
fend({'Global'  , ...
      'Local'  , ...
      'Global-Local' , ...
      'Local-Local'}, ...
     'fig/d1s16-f.eps','fmeasure');
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
                          sprintf('%04d',frame_out(f).number) ...
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
            fm = dlmread([data_out(i).path sprintf('%04d',imgnum) ...
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
            s = [s ste(t)];
            l{f} = int2str(frames(f).number);
        end
        
% $$$         eb = errorbar(1:length(m),m,s); 
        eb = errorbar(imgnums,m,s); 
        set(eb                            , ...
            'LineWidth'       , 1         , ...
            'Marker'          , 'o'       , ...
            'MarkerSize'      , 3         , ...      
            'MarkerFaceColor' , [.7 .7 .7]);
        if(~isempty(frames(1).color))
            set(eb,'Color',frames(1).color);
        end
    end
    
    xlim([0 length(imgnums)])
    
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
	set(findobj('type','axes'),'fontsize',20) 
    print('-depsc2', output);
end
