function plotANNInput(fileName, waveformPath, ifrom, ito, varargin)
plotEventFlag = aroptions('plot event flag', varargin{:});

VN2 = load(fileName);
[M,N] = size(VN2);
marg = 0.0025;
Ni = (N-5) / 3;
ifrom = ifrom + 1;
xtime = unix2date(VN2(ifrom:M-ito, 1));
timeLimit(1) = xtime(1);
timeLimit(2) = xtime(end);

positionVector = [0.05,0.05,0.90,0.04];
subplot('Position',positionVector);
if(plotEventFlag)
    oo = VN2(:, end:-1:end-3);
    oo(:,5) = ones(M,1);
    size(oo)
    pcolor(xtime, 1:5, log(abs(oo(ifrom:M-ito,:)')+1e-20));

else
    pcolor(xtime, 1:4, log(abs(VN2(ifrom:M-ito, end-1:-1:end-4)')+1e-20));
end
ax = gca;
shading flat;
xlim(timeLimit);
xlim(timeLimit);
datetick('x','MM:SS', 'keeplimits')
ax.YTick = [];
caxis([-5 5]);
ylabel('\eta')
xlabel('time')

positionVector(2) = positionVector(2) + positionVector(4) + marg;
positionVector(4) = 0.04;
subplot('Position',positionVector);
pcolor(xtime, 1:4, VN2(ifrom:M-ito, end-4:-1:end-7)');
ax = gca;
shading flat;
ax.YTick = [];
ax.XTick = [];
caxis([0 2]);
ylabel('\zeta')

positionVector(2) = positionVector(2) + positionVector(4) + marg;
positionVector(4) = 0.19;
subplot('Position',positionVector)

pcolor(xtime, 1:Ni, VN2(ifrom:M-ito, 4:3:end-4)')
ax = gca;
shading flat;
xlim(timeLimit);
ax.YTick = [];
ax.XTick = [];
ylabel('DOP')

positionVector(2) = positionVector(2) + positionVector(4) + marg;
positionVector(4) = 0.19;
subplot('Position',positionVector)
pcolor(xtime, 1:Ni, VN2(ifrom:M-ito, 3:3:end-4)')
ax = gca;
shading flat;
xlim(timeLimit);
ax.YTick = [];
ax.XTick = [];
ylabel('h.STA/LTA')

positionVector(2) = positionVector(2) + positionVector(4) + marg;
positionVector(4) = 0.19;
subplot('Position',positionVector)
pcolor(xtime, 1:Ni, VN2(ifrom:M-ito, 2:3:end-4)')
ax = gca;
shading flat;
xlim(timeLimit);
ax.YTick = [];
ax.XTick = [];
ylabel('v.STA/LTA')

k = strfind(fileName,'VN.');
waveformFileName = fileName(k:end-3);

labels = 'ENZ';
for idx = 1:3
    sv  = load([waveformPath '\' waveformFileName labels(idx)]);
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
    ax.YTick = [];
    ax.XTick = [];
    l = ylabel([labels(idx) '   ']);
    l.Rotation = 0.0
end

end
