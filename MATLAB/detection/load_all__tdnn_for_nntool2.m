function [odata] = load_all__tdnn_for_nntool2(path, varargin)
% function [nndata, odata] = load_all__tdnn_for_nntool2(path, ...)
%
% Preparation and computing detection in the Matlab Neural Network Tool
%
% Required inputs:
%   path  - path to directory with samples of seismic inputs (string)
%           It must end with the '\'
% Input parameters:
%   'station', sta - define the single station for reading data.

% Ouputs:
%   odata  - pure data read from files
%
% (c) Jan Wiszniowski, version 2019.01.28

    files = dir([path '*.dat']) ;
    nFiles = length(files)  ;
    odata = cell(nFiles,1) ;
    index = 0;
    for i=1:nFiles
        fprintf('Load %s %d/%d', files(i).name, i, nFiles)
        wyn = load_tdnn_for_nntool2(path, files(i).name, varargin{:}) ;
        if isempty(wyn.data)
            fprintf(' is empty\n')
        else
            index = index + 1 ;
            odata{index} = wyn ;
            fprintf(' is ok\n')
        end
    end
    odata = odata(1:index);
end

