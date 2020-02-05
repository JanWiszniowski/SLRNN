function [] = save_tdnn_weights( inet, fileName )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    file = fopen(fileName,'w');
    fprintf(file,'#SLRNN\n');
    fprintf(file,'#No inputs:\n%d\n',inet.inputWeights{1}.size(2));
    fprintf(file,'#No neurons:\n%d\n',inet.layerWeights{1}.size(1));
    fprintf(file,'#Delays:\n');
    fprintf(file,' %d', inet.layerWeights{1}.delays);
    fprintf(file,'\n');
    fprintf(file,'#Recurrent weights: %d x %d\n',inet.layerWeights{1}.size(1), inet.layerWeights{1}.size(2));
    for idx = 1:inet.layerWeights{1}.size(2)
        fprintf(file,' %f',inet.LW{1}(:,idx));
        fprintf(file,'\n');
    end
    fprintf(file,'#Bias weights: %d x 1\n',inet.layerWeights{1}.size(1));
    fprintf(file,' %f',inet.b{1});
    fprintf(file,'\n');
    fprintf(file,'#Input weights: %d x %d\n',inet.inputWeights{1}.size(1), inet.inputWeights{1}.size(2));
    for idx = 1:inet.inputWeights{1}.size(2)
        fprintf(file,' %f',inet.IW{1}(:,idx));
        fprintf(file,'\n');
    end
    fclose(file);
end

