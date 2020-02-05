function [] = plotANNOutputs( inet, nndata, varargin )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    n = size(nndata.in{1},2);
    for idx1 = 1:n
        fprintf('Sample %d/%d "%s"\n',idx1,n,nndata.name{idx1})
        plotANNOutput(inet, nndata, idx1, varargin{:})
        pause
    end

end

