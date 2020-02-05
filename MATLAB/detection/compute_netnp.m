function [odata,oerr] = compute_netnp(inet,nndata,notest)
    Xs = nndata.in ;
    Ts = nndata.out ;
    Ews = nndata.weight ;
    odata = cell(1,notest) ;
    oerr = NaN(1,notest) ;
    for i = 1:notest
        disp(['Start ' num2str(i)])
        inet = init(net) ;
        onet = train(inet,Xs,Ts,[],[],Ews,'useParallel','yes') ;
        y = onet(Xs) ;
        oerr(i) = perform(onet,Ts,y,Ews) ;
        fprintf('%d/%d err=%f, ',i,notest,oerr(i))
        odata{i} = onet ;
        save slrnntemp.mat odata oerr ;
    end
end
