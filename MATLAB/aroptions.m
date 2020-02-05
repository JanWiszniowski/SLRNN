function [parameter] = aroptions(parametername,varargin)
parameter = false ;
N = length(varargin) ;
for i = 1:length(varargin)
    if ischar(varargin{i})
        if strcmp(varargin{i},parametername)
            parameter = true ;
            return
        end
    end
end
