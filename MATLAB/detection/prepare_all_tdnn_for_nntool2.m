function [nndata, odata] = prepare_all_tdnn_for_nntool2(path, varargin)
% function [nndata, odata] = prepare_all_slrnn_for_nntoolN2(path)
%
% Preparation of training data for the Matlab Neural Network Tool
%
% Required inputs:
%   path  - path to directory with samples of seismic inputs (string)
%           It must end with the '\'
% Input parameters:
%   'pause'             - pauses program before each input file
%   'ANN size',nnsize    - define size of slrnn (number of neurons) (default 3)
%   'max random',value   - define number of samples of random movement
%   'cat at the begin',value    - define number of samples of samples removed at
%                          begin of data
%   'max length',value   - define maximum number of samples of samples
%   'random permutation' - random permutates the direction of training data
%
% Ouputs:
%   odata  - data read from files
%   nndata - data for NN training
%
% Use case
% [inndata] = prepare_all_slrnn_for_nntoolN('.\MATLAB\detection\preprocessed\','ANN size',16)
% 
%
% (c) Jan Wiszniowski, version 2019.01.16

    files = dir([path '*.dat']) ;
    nFiles = length(files)  ;
    index = 0 ;
    odata = cell(nfiles,1) ;
    inlength = 0 ;
    if aroptions('random permutation', varargin{:})
        files = files(randperm(nFiles));
    end
    for idxIterations = 1:noIterations
        for i=1:nfiles
            fprintf('%s %d/%d', files(i).name, i, nfiles)
            [wyn] = prepare_tdnn_for_nntool(path, files(i).name, picks, windowsize, maxcut, varargin{:}) ;
            if isempty(wyn.ins)
                fprintf(' is empty'])
            else
                index = index + 1 ;
                odata{index} = wyn ;
                inlength = max(inlength,length(wyn.time)) ;
            end
        end
        odata = odata(1:index) ;
        insize = size(odata{1}.ins,2) ;
        outsize = arparameters('osize', size(odata{1}.eouts,2), varargin{:}) ;
        fprintf('Input size %d x %d\n', insize, inlength) ;
        fprintf('Output size %d x %d\n', outsize, inlength) ;
        nndata.in = cell(1,inlength) ;
        nndata.out = cell(1,inlength) ;
        nndata.weight = cell(1,inlength) ;
        for idx1 = 1:inlength
            nndata.in{idx1} = zeros(insize,index) ;
            nndata.out{idx1} = zeros(outsize,index) ;
            nndata.weight{idx1} = zeros(outsize,index) ;
        end
        for idx1 = 1:index
            wyn = odata{idx1} ;
            for idx2=1:length(wyn.time)
                nndata.in{idx2}(:,idx1) = wyn.ins(idx2,:) ;
                nndata.out{idx2}(1:3,idx1) = wyn.eouts(idx2,:) ;
                nndata.weight{idx2}(1:3,idx1) = wyn.defaultetas(idx2,:) ;
            end
        end
    end
end

