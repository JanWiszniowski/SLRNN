function plotANNOutput(inet, nndata, idx1, varargin)
    cmapFlag = aroptions('cmap',varargin{:});
    netSize = size(nndata.out{1},1);
    nLength = numel(nndata.out);
    outpuSize = arparameters('output size',netSize,varargin{:});
    y = inet(nndata.in) ;
    eouts = zeros(outpuSize+1, nLength);
    outs = zeros(outpuSize+1, nLength);
    defaultetas = zeros(outpuSize+1, nLength);
    time = NaN(1, nLength);
    fileName = nndata.name{idx1};
    for idx2=1:nLength
%         nndata.in{idx2}(:,idx1) = wyn.ins(idx2,:) ;
        outs(1:outpuSize,idx2) = y{idx2}(1:outpuSize,idx1) ;
        eouts(1:outpuSize,idx2) = nndata.out{idx2}(1:outpuSize,idx1) ;
        defaultetas(1:outpuSize,idx2) = nndata.weight{idx2}(1:outpuSize,idx1) ;
        time(1, idx2) = nndata.time{idx2}(1,idx1);
    end
    marg = 0.0025;
    xtime = unix2date(time(~isnan(time)));
    timeLength = size(xtime,2);
    timeLimit(1) = xtime(1);
    timeLimit(2) = xtime(end);
    
    if cmapFlag
    % Plot cmaps
    
    % View eta
        positionVector = [0.05,0.05,0.90,0.04];
        subplot('Position',positionVector);
        pcolor(xtime, 1:4, log(abs(defaultetas(:,1:timeLength))+1e-20));
        ax = gca;
        shading flat;
        xlim(timeLimit);
        datetick('x','MM:SS', 'keeplimits')
        ax.YTick = [];
        caxis([-5 5]);
        ylabel('\eta')
        xlabel('time')

        % View expected out
        positionVector(2) = positionVector(2) + positionVector(4) + marg;
        positionVector(4) = 0.04;
        subplot('Position',positionVector);
        pcolor(xtime, 1:4, eouts(:,1:timeLength));
        ax = gca;
        shading flat;
        xlim(timeLimit);
        ax.YTick = [];
        ax.XTick = [];
        caxis([-1 1]);
        ylabel('o')

        % View real out
        positionVector(2) = positionVector(2) + positionVector(4) + marg;
        positionVector(4) = 0.04;
        subplot('Position',positionVector);
        pcolor(xtime, 1:4, outs(:,1:timeLength));
        ax = gca;
        shading flat;
        xlim(timeLimit);
        ax.YTick = [];
        ax.XTick = [];
        caxis([-1 1]);
        ylabel('o')
    else
        kolor = 'rbg';
        positionVector = [0.05,0.05,0.90,0.1];
        subplot('Position',positionVector);
        hold off
        for pidx = 1:3
            plot(xtime, log10(defaultetas(pidx,1:timeLength)+1e-2)+0.1*(pidx-1),kolor(pidx));
            hold on
        end
        ax = gca;
        xlim(timeLimit);
        datetick('x','MM:SS', 'keeplimits')
        ylim([-2.2 2]);
        ax.YTick = [-2 0 1 log10(50)];
        ax.YTickLabel = {'ign' 'imp' 'v.i' 'e.i'};
        ylabel('\eta')
        xlabel('time')

        % View expected out
        positionVector(2) = positionVector(2) + positionVector(4) + marg;
        subplot('Position',positionVector);
        hold off
        for pidx = 1:3
            plot(xtime, eouts(pidx,1:timeLength)+0.1*(pidx-1),kolor(pidx));
            hold on
        end
        ax = gca;
        xlim(timeLimit);
        datetick('x','MM:SS', 'keeplimits')
        ax.XTickLabel = {};
        ylim([-1.2 1.5]);
        ylabel('o')

        % View real out
        positionVector(2) = positionVector(2) + positionVector(4) + marg;
        subplot('Position',positionVector);
        hold off
        for pidx = 1:3
            plot(xtime, outs(pidx,1:timeLength)+0.1*(pidx-1),kolor(pidx));
            hold on
        end
        ax = gca;
        xlim(timeLimit);
        datetick('x','MM:SS', 'keeplimits')
        ax.XTickLabel = {};
        ylim([-1.2 1.5]);
        ylabel('o')
    end

    k = strfind(fileName,'VN.');
    waveformFileName = fileName(k:end-3);
    waveformPath = arparameters('waveform path',[]',varargin{:});
    if isempty(waveformPath)
        return
    end
    labels = 'ENZ';
    for idx = 1:3
        filename = [waveformPath '\' waveformFileName labels(idx)];
        if exist(filename, 'file')
            sv  = load(filename);
            positionVector(2) = positionVector(2) + positionVector(4) + marg;
            positionVector(4) = 0.08;
            subplot('Position',positionVector)
            stime = unix2date(sv(:,1));
            iidx = (stime >=  timeLimit(1)) & (stime <=  timeLimit(2));
            slimits(2) = max(sv(iidx,2));
            slimits(1) = min(sv(iidx,2));
            delta = (slimits(2) - slimits(1)) * 0.1;
            slimits(1) = slimits(1) - delta;
            slimits(2) = slimits(2) + delta;
            plot(stime(iidx,1),sv(iidx,2),'k');
            ylim(slimits);
            ax = gca;
            xlim(timeLimit);
            datetick('x','MM:SS', 'keeplimits')
            ax.XTickLabel = {};
            ax.YTick = [];
            l = ylabel([labels(idx) '   ']);
            l.Rotation = 0.0;
        else
            warning('Missing file %s\n', filename)
    end
    title(waveformFileName(1:end-1))
end
