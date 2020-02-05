function [onet,oerr] = compute_tdnn(inet, nndata, varargin)
    Xs = nndata.in ;
    Ts = nndata.out ;
    Ews = nndata.weight ;
    fprintf('Start traning\n');
    useParallel = arparameters('useParallel','yes',varargin{:});
    onet = train(inet,Xs,Ts,[],[],Ews,'useParallel',useParallel) ;
    y = onet(Xs) ;
    oerr = perform(onet,Ts,y,Ews) ;
end
