function [ file ] = plotAllANNinputs( path, mask, waveformPath, ifrom, ito, varargin)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here

file = dir([path '/' mask]);
N = numel(file);
for idx = 1:N
    if(~file(idx).isdir)
        fileName = [path '/' file(idx).name];
        fprintf('%d/%d: Press pause to plot.',idx,N)
        pause
        plotANNInput(fileName, waveformPath, ifrom, ito, varargin{:});
        fprintf(' File: "%s".\n',fileName)
    end
end

end

