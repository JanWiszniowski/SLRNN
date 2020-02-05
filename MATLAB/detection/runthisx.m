% runthisx
load LaiChauSLRNN_20190925_br_narx_init
parpool(4)

load LaiChauSLRNN_20190606 pureData
[onetAll,oerrAll] = compute_tdnn_for_nntool2(inetX, pureData, 'no iterations',1 , 'max random',25, 'cat at the begin', 65, 'max length', 300, 'no tests',3, 'no kept tests',2);
save SRNN_LAICHAU_temp
