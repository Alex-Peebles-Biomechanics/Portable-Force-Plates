function [aveVelocity, swayArea] = Analyze_Balance(DataIn, plottitle, Sys)
time = DataIn(:,1)/1000;
% Filter data
Fs = 1/(mean(diff(time)));
Fc = 6;
[b,a] = butter(4,Fc/(Fs/2));
CoP_ML = filtfilt(b,a,DataIn(:,2));
CoP_AP = filtfilt(b,a,DataIn(:,3));

figure(1)
if Sys > 0
subplot(3,1,Sys)
end
% plot(CoP_ML,CoP_AP,'b')
% xlabel('Mediolateral position (cm)')
% xlabel('Anteroposterior position (cm)')
% title(plottitle)

plot(time,CoP_ML,'r',time,CoP_AP,'b')
xlabel('time (s)')
ylabel('Position (cm)')
legend('ML','AP')
title(plottitle)

swayVelocity = nan(length(CoP_AP)-2,1);
for i = 2:length(CoP_AP)-1
    swayVelocity(i-1) = sqrt((CoP_ML(i+1)-CoP_ML(i-1))^2 + (CoP_AP(i+1)-CoP_AP(i-1))^2)/(time(i+1)-time(i-1));
end
aveVelocity = mean(swayVelocity);

% Compute covariance matrix
C = cov(CoP_AP, CoP_ML);

% Extract eigenvalues
e = eig(C);
lambda1 = e(1);
lambda2 = e(2);

% Chi-square value for 95% confidence in 2D
chi95 = 5.991;

% Compute 95% confidence ellipse area
swayArea = pi * chi95 * sqrt(lambda1 * lambda2);
