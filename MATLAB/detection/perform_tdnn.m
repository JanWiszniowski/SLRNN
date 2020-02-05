function [oerr] = perform_tdnn(inet, nndata)
    Xs = nndata.in ;
    Ts = nndata.out ;
    Ews = nndata.weight ;
    y = inet(Xs) ;
    oerr = perform(inet,Ts,y,Ews) ;
end
