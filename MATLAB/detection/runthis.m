addpath detection
load LaiChauSLRNN_20190219
% [onetMLAV,oerrMLAV] = compute_tdnn_for_nntool2(net_16, pureDataMLAV, 'no iterations',10, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',20, 'no kept tests',7);
% save SRNN_LAICHAU_temp
[onetCCA,oerrCCA] = compute_tdnn_for_nntool2(znet_16, pureDataCCA, 'no iterations',10, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',20, 'no kept tests',7);
save SRNN_LAICHAU_temp
[onetMTE,oerrMTE] = compute_tdnn_for_nntool2(net_16, pureDataMTE, 'no iterations',10, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',20, 'no kept tests',7);
save SRNN_LAICHAU_temp
[onetNNU,oerrNNU] = compute_tdnn_for_nntool2(net_16, pureDataNNU, 'no iterations',10, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',20, 'no kept tests',7);
save SRNN_LAICHAU_temp