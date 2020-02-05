purefunction [odata] = prepare_tdnn_for_nntool2(path, datafile, varargin)
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
    if aroptions('station', varargin{:})
        sel_sta = arparameters('station', '', varargin{:});
        if ~strncmp(datafile,sel_sta, lenfth(sel__sta))
            disp(['Station ' sel_sta ' ignored']) 
            odata.ins = NaN ;
            return ;
        end
    end
    maxRandom = arparameters('max random', 0, varargin{:});
    maxLength = arparameters('max length', 0, varargin{:});
    catBegin = arparameters('cat at the begin', 0, varargin{:});
    fileName = [path datafile];
    data = load(fileName);
    [dataN, dataM] = size(data);
    dataFrom = 1 + catBegin + floor(rand() * maxRandom);
    if maxLength == 0
        dataTo = dataN;
    else
        dataTo = min(dataFrom+maxLength,dataN);
    end
    
    if aroptions('pause', varargin{:})
        waveformPath = path;
        if aroptions('station', varargin{:})
            waveformPath = arparameters('pause', path, varargin{:});
        end
        plotANNInput(fileName, waveformPath, dataFrom, dataN-dataTo)
        fprintf('%s: %d - %d\n', datafile, dataFrom, dataTo);
    end
    
    odata = struct('ins',[],'eouts',[],'defaultetas',[],'time',[]);
    odata.ins = data(dataFrom:dataTo, 2:(dataM-7));
    odata.eouts = data(dataFrom:dataTo,(dataM-7):(dataM-4)) * 2.0 - 1.0;
    odata.defaultetas = data(dataFrom:dataTo,(dataM-3):(dataM-1));
    odata.time = data(dataFrom:dataTo,(dataM-3):(dataM-1));
end
