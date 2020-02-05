function [outs] = ANNDetTestMat(net,  varargin)
    N = 100;
    Xf =[];
    Af = [];
    outs = nan(N,16);
    X = ones(1,42);
    exnet = net;
    LW = zeros(16,64);
    LW(2,1) = 1;
    LW(3,17) = 1;
    LW(4,33) = 1;
    LW(5,49) = 1;
    exnet.LW{1} = LW
    exnet.IW{1} = zeros(16,42)
    exnet.b{1} = [1; zeros(15,1)]

    for idx = 1:N
        fprintf('X=')
        fprintf(' %f', X)
        fprintf('\n')
        [Y,Xf,Af] = sim(exnet,X',Xf,Af);
        fprintf('Y=')
        fprintf(' %f', Y)
        fprintf('\n')
        for idx2 = 1:8
            fprintf(' %f', Af{idx2})
            fprintf('\n')
        end
        pause
        outs(idx,:) = Y;
        X = zeros(1,42);
    end
%     marg = 0.0025;
%     xtime = unix2date(out(:,1));
%     timeLimit(1) = xtime(1);
%     timeLimit(2) = xtime(end);
%     
%     kolor = 'kbg';
%     positionVector = [0.05,0.05,0.90,0.1];
%     subplot('Position',positionVector);
%     hold off
%     for pidx = 1:3
%         plot(xtime, out(:,pidx+1)+0.1*(pidx-1),kolor(pidx));
%         hold on
%     end
%     xlim(timeLimit);
%     datetick('x','MM:SS', 'keeplimits')
%     ylim([-1.1 1.1]);
%     xlabel('time')
% 
%     labels = 'ENZ';
%     for idx = 1:3
%         sv  = load([path datafile '.' labels(idx)]);
%         positionVector(2) = positionVector(2) + positionVector(4) + marg;
%         positionVector(4) = 0.08;
%         subplot('Position',positionVector)
%         stime = unix2date(sv(:,1));
%         iidx = (stime >=  timeLimit(1)) & (stime <=  timeLimit(2));
%         slimits(2) = max(sv(iidx,2));
%         slimits(1) = min(sv(iidx,2));
%         delta = (slimits(2) - slimits(1)) * 0.1;
%         slimits(1) = slimits(1) - delta;
%         slimits(2) = slimits(2) + delta;
%         plot(stime(iidx,1),sv(iidx,2),'k');
%         ylim(slimits);
%         ax = gca;
%         xlim(timeLimit);
%         datetick('x','MM:SS', 'keeplimits')
%         ax.XTickLabel = {};
%         ax.YTick = [];
%         l = ylabel([labels(idx) '   ']);
%         l.Rotation = 0.0;
%     end
end
