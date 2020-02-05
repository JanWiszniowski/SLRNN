function [parameter] = arparameters(parametername,defaultvalue,varargin)
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
