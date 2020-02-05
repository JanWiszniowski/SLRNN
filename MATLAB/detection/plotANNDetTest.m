% function plotANNDetTest(ipath, datafile, varargin) 
% Prezentacja wynikow liczonych programem SWIP5
function plotANNDetTest(ipath, datafile, varargin) 
    outs = load([ipath datafile '.wyn']);
    marg = 0.0025;
    xtime = unix2date(outs(:,1));
    timeLimit(1) = xtime(1) + arparameters('begin',0,varargin{:}) / 86400.0;
    timeLimit(2) = xtime(end) - arparameters('end',0,varargin{:}) / 86400.0;
    
    kolor = 'rbg';
    positionVector = [0.05,0.05,0.90,0.1];
    subplot('Position',positionVector);
    hold off
    for pidx = 3:-1:1
        plot(xtime, outs(:,pidx+1)+0.1*(pidx-1),kolor(pidx));
        hold on
    end
    xlim(timeLimit);
    datetick('x','MM:SS', 'keeplimits')
    ylim([-1.1 1.1]);
    xlabel('time')

    labels = 'ENZ';
    for idx = 1:3
        sv  = load([ipath datafile '.' labels(idx)]);
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
