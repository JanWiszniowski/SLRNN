function [parameter] = arispar(parametername,varargin)
% function [parameter] = aroptions(parametername,...,parName1,...)
% Checks if the parametername exist in the list of function call parameters following
% the parametername. If exist the name and corresponding value,
% the function returns true. Otherworld, it returns false.
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
