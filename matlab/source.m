series = 1;

%volume = 'seq2/';
%volume = 'seq3/';
volume = 'seq1/';

% rawpath = [volume 'data/new/raw/4000_Series/'];
% rawpath = [volume 'data/old/scaled/'];
rawpath = [volume 'img/'];
datapath = [volume 'img/'];
%datapath = 'data/old/scaled/';
%datapath = 'img/';
labelpath = [volume 'labels/'];
regionpath = [volume 'region/'];
edgepath = [volume 'edge/'];
outputpath = [volume 'output/'];
%groundpath = [volume 'ground/'];
groundpath = [volume 'img/ground/'];
groundimgpath = [groundpath 'img/'];
labeltype = 'label';
scoretype = 'score';
inputimgtype = 'png';
imgtype = 'png';
% prefix = '4000_image';
prefix = 'image';
postfix = '';

w1path = 'watershed/';
%w1path = 'outputw/';
w1labelpath = [volume w1path 'labels/'];
w1regionpath = [volume w1path 'region/'];
w1edgepath = [volume w1path 'edge/'];
w1outputpath = [volume w1path 'output/'];

w2path = 'watershed2/';
%w2path = 'outputw2/';
w2labelpath = [volume w2path 'labels/'];
w2regionpath = [volume w2path 'region/'];
w2edgepath = [volume w2path 'edge/'];
w2outputpath = [volume w2path 'output/'];

n1path = 'nc/';
n1labelpath = [volume n1path 'labels/'];
n1regionpath = [volume n1path 'region/'];
n1edgepath = [volume n1path 'edge/'];
n1outputpath = [volume n1path 'output/'];

n2path = 'nc2/';
n2labelpath = [volume n2path 'labels/'];
n2regionpath = [volume n2path 'region/'];
n2edgepath = [volume n2path 'edge/'];
n2outputpath = [volume n2path 'output/'];
