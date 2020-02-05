function [ f ] = loadiir( fname )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
file = fopen(fname,'r');
f.name = fgets(file);
n = fscanf(file,'%d',1);
f.b = fscanf(file,'%f',n);
n = fscanf(file,'%d',1);
f.a = fscanf(file,'%f',n);
fclose(file);
end

