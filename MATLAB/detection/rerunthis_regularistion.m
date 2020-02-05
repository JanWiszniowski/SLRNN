addpath detection
load LaiChauSLRNN_20190923_br_init

parpool(4)

load LaiChauSLRNN_20190606 pureData

[onetAll,oerrAll] = recompute_tdnn_for_nntool2(inetAll, pureData, 'no iterations',3 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',4, 'no kept tests',3);
clear pureData
save SRNN_LAICHAU_BR
