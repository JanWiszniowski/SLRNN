function [onet,oerr] = recompute_tdnn_for_nntool2(inet, indata, varargin)
% function [nndata, odata] = prepare_all_slrnn_for_nntoolN2(path)
%
% Preparation and recomputing detection in the Matlab Neural Network Tool
%
% Required inputs:
%   inet - initial networks
%   indata - pure input data (full tranning set)
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
% (c) Jan Wiszniowski, version 2019.03.26


    noIterations = arparameters('no iterations',1,varargin{:});
    noTests = arparameters('no tests',length(inet),varargin{:});
    if noTests > length(inet)
        noTests = length(inet);
    end
    noKept = arparameters('no kept tests',floor(noTests/3),varargin{:})+1;
    oerr = zeros(noIterations,noTests);
    onet = inet(1:noTests);
    if ischar(indata)
        indata = load_all_tdnn_for_nntool2(indata, varargin{:});
    end
    
    for idxIterations = 1:noIterations
        terr = zeros(1,noTests);
        for idxTest = 1:noTests
            fprintf('Iteration %d/%d test %d/%d\n', idxIterations, noIterations, idxTest, noTests)
            nndata = pre_all_tdnn_for_nntool2(inet{1}, indata, varargin{:});
            [onet{idxTest},terr(idxTest)] = compute_tdnn(onet{idxTest}, nndata, varargin{:});
        end
        
        oerr(idxIterations,:) = terr;
        [~,ierr] = sort(terr);
        onet = onet(ierr);
        oerr = oerr(:,ierr);
        for idxKept= noKept:noTests
            onet{idxKept} = init(inet{1});
            oerr(idxIterations,idxKept) = -oerr(idxIterations,idxKept);
        end
        save slrnntemp.mat onet oerr ;
        fprintf('End %d/%d err:',idxIterations, noIterations)
        fprintf(' %.1e', oerr(idxIterations, :));
        fprintf('\n')
    end
end

