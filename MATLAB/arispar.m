function [parameter] = arispar(parametername,varargin)
parameter = 0 ;
N = length(varargin) ;
for i = 1:length(varargin)-1
    if ischar(varargin{i})
        if strcmp(varargin{i},parametername)
            parameter = 1 ;
            return
        end
    end
end
