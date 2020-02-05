function [outs] = plotANNDetTestMat(net, path, datafile, varargin)
    wfpath = arparameters('waveform path',path,varargin{:});

    out = load([path datafile '.dat']);

    N = size(out,1)
    Xf =[];
    Af = [];
    outs = nan(N,16);
    for idx = 1:N
         X = out(idx,2:(end-7));
%         fprintf('X=')
%         fprintf(' %f', X)
%         fprintf('\n')
        [Y,Xf,Af] = sim(net,X',Xf,Af);
%         fprintf('Y=')
%         fprintf(' %f', Y)
%         fprintf('\n')
%         for idx2 = 1:8
%             fprintf(' %f', Af{idx2})
%             fprintf('\n')
%         end
%         pause
        outs(idx,:) = Y;
    end
    marg = 0.0025;
    xtime = unix2date(out(:,1));
    timeLimit(1) = xtime(1);
    timeLimit(2) = xtime(end);
    
    kolor = 'bgr';
    positionVector = [0.05,0.05,0.90,0.1];
    subplot('Position',positionVector);
    hold off
    for pidx = 1:3
        plot(xtime, out(:,pidx+1)+0.1*(pidx-1),kolor(pidx));
        hold on
    end
    xlim(timeLimit);
    datetick('x','MM:SS', 'keeplimits')
    ylim([-1.1 1.1]);
    xlabel('time')

    labels = 'ZNE';
    for idx = 1:3
        sv  = load([wfpath datafile '.' labels(idx)]);
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
    end
end
