clc;
clear;

T = readtable('motor-test.csv');

duty = T.duty_percent(:);
rpm  = T.delta(:) * 60 / (1400 * 0.01);

clear T;