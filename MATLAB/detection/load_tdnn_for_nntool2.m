function [odata] = load_tdnn_for_nntool2(path, datafile, varargin)
% function [odata] = prepare_tdnn_for_nntool2(path, datafile, ...)
%
% Reading one sample of training data for the Matlab Neural Network Tool
% from the file.
% The training data are prepared by the SWIP5 Export ANN Detection plugin.
%
% Inputs:
%   path - path do datafiles
%   datafile - name of file with data
%
% Input parameters:
%   'station',sta               - read only stations with the code sta.
%   'max random',value          - define number of samples of random movement
%   'cat at the begin',value    - define number of samples of samples removed at
%                                 begin of data
%   'max length',value          - define maximum number of samples of samples
%
% Output - structure of SLRNN data
%
% (c) Jan Wiszniowski, version 2019.01.16
%
    odata.name = datafile;
    if aroptions('station', varargin{:})
        sel_sta = arparameters('station', '', varargin{:});
        if ~strncmp(datafile,sel_sta, length(sel_sta))
            odata.data = [] ;
            return ;
        end
    end
    fileName = [path datafile];
    odata.data = load(fileName);
end
