function [fm,p,r] = fmeasure(gt,t,d)
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