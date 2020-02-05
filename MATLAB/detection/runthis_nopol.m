addpath detection
load LaiChauSLRNN_nopolarisation_20190805 net_16 
parpool(4)

load LaiChauSLRNN_nopolarisation_20190805 pureDataAll
[onetAll,oerrAll] = compute_tdnn_for_nntool2(net_16, pureDataAll, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',4, 'no kept tests',2);
clear pureDataAll
save SRNN_LAICHAU_temp

poolobj = gcp('nocreate');
delete(poolobj);
parpool(12)

load LaiChauSLRNN_nopolarisation_20190805 pureDataMLAV
[onetMLAV,oerrMLAV] = compute_tdnn_for_nntool2(net_16, pureDataMLAV, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataMLAV
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataCCA
[onetCCA,oerrCCA] = compute_tdnn_for_nntool2(net_16, pureDataCCA, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataCCA
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataMTE
[onetMTE,oerrMTE] = compute_tdnn_for_nntool2(net_16, pureDataMTE, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataMTE
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataNNU
[onetNNU,oerrNNU] = compute_tdnn_for_nntool2(net_16, pureDataNNU, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataNNU
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataCNUD
[onetCNUD,oerrCNUD] = compute_tdnn_for_nntool2(net_16, pureDataCNUD, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataCNUD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataHUBD, 
[onetHUBD,oerrHUBD] = compute_tdnn_for_nntool2(net_16, pureDataHUBD, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataHUBD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataKHOD, 
[onetKHOD,oerrKHOD] = compute_tdnn_for_nntool2(net_16, pureDataKHOD, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataKHOD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataPUDD
[onetPUDD,oerrPUDD] = compute_tdnn_for_nntool2(net_16, pureDataPUDD, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataPUDD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_nopolarisation_20190805 pureDataNNA3
[onetNNA3,oerrNNA3] = compute_tdnn_for_nntool2(net_16, pureDataNNA3, 'no iterations',4, 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests', 6, 'no kept tests',4);
clear pureDataNNA3
save SRNN_LAICHAU_temp


