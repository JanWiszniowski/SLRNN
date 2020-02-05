function [onet,oerr] = compute_tdnn_for_nntool2(inet, indata, varargin)
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


    noIterations = arparameters('no iterations',1,varargin{:});
    noTests = arparameters('no tests',1,varargin{:});
    noKept = arparameters('no kept tests',floor(noTests/3),varargin{:})+1;
    oerr = zeros(noIterations,noTests);
    onet = cell(1,noTests);
    for idxTest = 1:noTests
        onet{idxTest} = init(inet);
    end
    if ischar(indata)
        indata = load_all_tdnn_for_nntool2(indata, varargin{:});
    end
    
    for idxIterations = 1:noIterations
        terr = zeros(1,noTests);
        for idxTest = 1:noTests
            fprintf('Iteration %d/%d test %d/%d\n', idxIterations, noIterations, idxTest, noTests)
            nndata = pre_all_tdnn_for_nntool2(inet, indata, varargin{:});
            [onet{idxTest},terr(idxTest)] = compute_tdnn(onet{idxTest}, nndata);
            onet{idxTest}.performParam.regularization = 0.08 + onet{idxTest}.performParam.regularization * 0.9
        end
        
        oerr(idxIterations,:) = terr;
        [~,ierr] = sort(terr);
        onet = onet(ierr);
        oerr = oerr(:,ierr);
        for idxKept= noKept:noTests
            onet{idxKept} = init(inet);
            oerr(idxIterations,idxKept) = -oerr(idxIterations,idxKept);
        end
        save slrnntemp.mat onet oerr ;
        fprintf('End %d/%d err:',idxIterations, noIterations)
        fprintf(' %.1e', oerr(idxIterations, :));
        fprintf('\n')
    end
end

