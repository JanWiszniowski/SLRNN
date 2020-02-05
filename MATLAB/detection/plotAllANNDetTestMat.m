% function [] = plotAllANNDetTestMat(net, path, varargin)
% Wyswietla wyniki detekcji dla wszystkich plików w katalogu
function [] = plotAllANNDetTestMat(net, path, varargin)
    files = dir([path '*.dat']) ;
    nFiles = length(files)  ;
    for i=1:nFiles
        datafile = files(i).name(1:end-4);
        if aroptions('station', varargin{:})
            sel_sta = arparameters('station', '', varargin{:});
            if ~strncmp(datafile, sel_sta, length(sel_sta))
                 fprintf('Ignore %s %d/%d\n', datafile, i, nFiles)
                continue ;
            end
        end

        fprintf('Load %s %d/%d\n', datafile, i, nFiles)
        plotANNDetTestMat(net, path, datafile, varargin{:});
        pause;
    end
end
