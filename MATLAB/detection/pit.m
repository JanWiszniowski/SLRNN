function [ ot ] = pit( it, is, samples, kp, ki, kd )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    ot = zeros(size(it));
    ds = 0;
    intdt = 0 ;
    odt = 0;
    N = numel(it);
    ot(1) = it(1);
    for idx = 1:N-1
        lds = ds;
        if lds < -0.00001
            lds = -0.00001;
        end
        if lds > 0.00001
            lds = 0.00001;
        end
        ot(idx+1) = ot(idx) + (samples+lds) * is(idx);
        dt = (it(idx+1) - ot(idx+1))  / is(idx);
        
        intdt = intdt + dt;
        ds = kp * dt + intdt * ki * is(idx) + kd * (dt -odt);
        odt = dt;
    end

end

