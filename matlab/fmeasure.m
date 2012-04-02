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
   % Zero out if scores come back as NaN
   if isnan(fm)
       fm = 0;
   end
   if isnan(p)
       p = 0;
   end
   if isnan(r)
       r = 0;
   end
end

function n = normimg(im)
    n = uint8(im/max(max(im))*255);
end