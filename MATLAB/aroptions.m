function [parameter] = aroptions(parametername,varargin)
% function [parameter] = aroptions(parametername,parName1, parValue1,...)
% Checks if the parametername exist in the list of pairs ...,parName*, parValue*,...
% following the parametername. If exist the name and corresponding value,
% the function returns true. Otherword, it returns false.
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
