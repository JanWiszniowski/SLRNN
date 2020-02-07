function [parameter] = arparameters(parametername,defaultvalue,varargin)
% function [parameter] = arparameters(parametername,defaultvalue,parName1,parValue1,...)
% Checks if the parametername exist in the list of function call pairs
% of parameters ...,parName*, parValue*,... following the defaultvalue.
% If exist the name and corresponding value, the function returns
% the corresponding value. Otherword, it returns defaultvalue.

parameter = defaultvalue ;
N = length(varargin) ;
for i = 1:length(varargin)
    if ischar(varargin{i})
        if strcmp(varargin{i},parametername) && i < N
            parameter = varargin{i+1} ;
            return
        end
    end
end
