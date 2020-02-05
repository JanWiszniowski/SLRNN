function [] = plotiirs( ipath )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here
    a = dir(ipath);
%     hold off
    n = numel(a);
    for idx = 3:n
        if ~a(idx).isdir
            f = loadiir([ipath '\' a(idx).name]);
            w1 = filter(f.b,f.a,ones(100,1));
            subplot(n-2,1,idx-2)
            plot(w1)
        end
    end

end

