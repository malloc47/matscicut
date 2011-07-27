series = 1;

volume = 'seq2/';

% rawpath = [volume 'data/new/raw/4000_Series/'];
% rawpath = [volume 'data/old/scaled/'];
rawpath = [volume 'img/'];
datapath = [volume 'img/'];
%datapath = 'data/old/scaled/';
%datapath = 'img/';
labelpath = [volume 'watershed/labels/'];
regionpath = [volume 'watershed/region/'];
edgepath = [volume 'watershed/edge/'];
outputpath = [volume 'watershed/output/'];
groundpath = [volume 'ground/'];
groundimgpath = [groundpath 'img/'];
labeltype = 'label';
inputimgtype = 'png';
imgtype = 'png';
% prefix = '4000_image';
prefix = 'image';
postfix = '';