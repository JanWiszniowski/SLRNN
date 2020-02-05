addpath detection
load LaiChauSLRNN_20190606_init

parpool(4)

load LaiChauSLRNN_20190606 pureData
[onetAll,oerrAll] = recompute_tdnn_for_nntool2(inetAll, pureData, 'no iterations',3 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',4, 'no kept tests',3);
clear pureData
save SRNN_LAICHAU_temp

poolobj = gcp('nocreate');
delete(poolobj);
parpool(12)

load LaiChauSLRNN_20190606 pureDataCCA
[onetCCA,oerrCCA] = recompute_tdnn_for_nntool2(inetCCA, pureDataCCA, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataCCA
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataCNUD
[onetCNUD,oerrCNUD] = recompute_tdnn_for_nntool2(inetAll, pureDataCNUD, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataCNUD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataDBVB
[onetDBVB,oerrDBVB] = recompute_tdnn_for_nntool2(inetAll, pureDataDBVB, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataDBVB
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataHUBD
[onetHUBD,oerrHUBD] = recompute_tdnn_for_nntool2(inetAll, pureDataHUBD, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataHUBD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataMLAV
[onetMLAV,oerrMLAV] = recompute_tdnn_for_nntool2(inetMLAV, pureDataMLAV, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataMLAV
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataMTE
[onetMTE,oerrMTE] = recompute_tdnn_for_nntool2(inetMTE, pureDataMTE, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataMTE
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataNNA3
[onetNNA3,oerrNNA3] = recompute_tdnn_for_nntool2(inetNNA3, pureDataNNA3, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataNNA3
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataNNU
[onetNNU,oerrNNU] = recompute_tdnn_for_nntool2(inetNNU, pureDataNNU, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataNNU
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataKHOD
[onetKHOD,oerrKHOD] = recompute_tdnn_for_nntool2(inetAll, pureDataKHOD, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataKHOD
save SRNN_LAICHAU_temp

load LaiChauSLRNN_20190606 pureDataPUDD
[onetPUDD,oerrPUDD] = recompute_tdnn_for_nntool2(inetAll, pureDataPUDD, 'no iterations',5 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',6, 'no kept tests',4);
clear pureDataPUDD
save SRNN_LAICHAU_temp
