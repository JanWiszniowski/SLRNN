function [nndata] = pre_all_tdnn_for_nntool2(net, puredata, varargin)
% function [nndata, odata] = prepare_all_slrnn_for_nntoolN2(path)
%
% Preparation and computing detection in the Matlab Neural Network Tool
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
    printOption = aroptions('verbose', varargin{:});
    nData = numel(puredata);
    odata = cell(nData,1) ;
    indexes = randperm(nData);
    inLength = 0 ;
    for idx=1:nData
        odata{idx} = pre_tdnn_for_nntool2(puredata{indexes(idx)}, varargin{:});
        inLength = max(inLength,length(odata{idx}.time)) ;
    end
    inSize = size(odata{1}.ins,2) ;
    outSize = size(odata{1}.eouts,2);
    etaSize = size(odata{1}.defaultetas,2);
    netSize = net.outputs{1}.size ;
    if printOption
        fprintf('Input size %d x %d\n', inSize, inLength) ;
        fprintf('Output size %d x %d\n', outSize, inLength) ;
        fprintf('Eta size %d x %d\n', etaSize, inLength) ;
        fprintf('Neurons size %d\n', netSize) ;
        fprintf('Samples %d\n', nData) ;
    end
    if outSize ~= etaSize || outSize > netSize || inSize ~= net.inputs{1}.size
        error('Wrong sizes')
    end
    nndata.name = cell(1,nData);
    nndata.in = cell(1,inLength) ;
    nndata.out = cell(1,inLength) ;
    nndata.weight = cell(1,inLength) ;
    nndata.time = cell(1,inLength) ;
    for idx1 = 1:inLength
        nndata.in{idx1} = zeros(inSize,nData) ;
        nndata.out{idx1} = zeros(netSize,nData) ;
        nndata.weight{idx1} = zeros(netSize,nData) ;
        nndata.time{idx1} = NaN(1,nData) ;
    end
    for idx1 = 1:nData
        wyn = odata{idx1} ;
        nndata.name{idx1} = wyn.name; 
        for idx2=1:length(wyn.time)
            nndata.in{idx2}(:,idx1) = wyn.ins(idx2,:) ;
            nndata.out{idx2}(1:outSize,idx1) = wyn.eouts(idx2,:) ;
            nndata.weight{idx2}(1:etaSize,idx1) = wyn.defaultetas(idx2,:) ;
            nndata.time{idx2}(1,idx1) = wyn.time(idx2,1);
        end
    end
end

