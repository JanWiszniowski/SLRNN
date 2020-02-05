function [odata] = pre_tdnn_for_nntool2(indata, varargin)
% function [odata] = prepare_tdnn_for_nntool2(path, datafile, ...)
%
% Reading one sample of training data for the Matlab Neural Network Tool
% from the file.
% The training data are prepared by the SWIP5 Export ANN Detection plugin.
%
% Inputs:
%   path - path do datafiles
%   data - pure data fromthe file
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
    maxRandom = arparameters('max random', 0, varargin{:});
    maxLength = arparameters('max length', 0, varargin{:});
    catBegin = arparameters('cat at the begin', 0, varargin{:});
    [dataN, dataM] = size(indata.data);
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
    
    odata = struct('ins',[],'eouts',[],'defaultetas',[],'time',[],'file',[]);
    odata.name = indata.name;
    odata.ins = indata.data(dataFrom:dataTo, 2:(dataM-7));
    odata.eouts = indata.data(dataFrom:dataTo,(dataM-6):(dataM-4)) * 2.0 - 1.0;
    if dataTo < dataN
        if any(indata.data((dataTo+1):end,(dataM-6)) > 0.0)
            fprintf('Detection outside limit\n')
        end
        if any(indata.data((dataTo+1):end,(dataM-5)) > 0.0)
            fprintf('P outside limit\n')
        end
        if any(indata.data((dataTo+1):end,(dataM-4)) > 0.0)
            fprintf('S outside limit\n')
        end
    end
    if any(indata.data((dataFrom+31):end,(dataM-4):(dataM-5)) > 0.0)
        fprintf('Begin on the runway\n')
    end
    odata.defaultetas = indata.data(dataFrom:dataTo,(dataM-3):(dataM-1));
    odata.defaultetas(1:40,:) = 0.0;
    odata.time = indata.data(dataFrom:dataTo,1);
end
