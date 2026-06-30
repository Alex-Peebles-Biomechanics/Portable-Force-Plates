function [Imp_ASI, Pk_ASI] = Analyze_Squat(DataIn, plottitle, Sys)
time = DataIn(:,1)/1000;
% Filter data
Fs = 1/mean(diff(time));
Fc = 50;
[b,a] = butter(4,Fc/(Fs/2));
leftForce = filtfilt(b,a,DataIn(:,2));
rightForce = filtfilt(b,a,DataIn(:,3));


L_Imp = trapz(leftForce);
R_Imp = trapz(rightForce);
Imp_ASI = 100*(R_Imp-L_Imp)/mean([L_Imp,R_Imp],2);

totalForce = leftForce + rightForce;
Fc = 2;
[b,a] = butter(4,Fc/(Fs/2));
totalForce = filtfilt(b,a,totalForce);

BW = mean(totalForce(1:50));

[pks, locs] = findpeaks(totalForce,"MinPeakHeight",BW*1.1,"MinPeakDistance",150);
% If no peaks, then just present max overall for L/R

% delete peaks in first or last 50 points
pks(locs<60) = [];
locs(locs<60) = [];
pks(locs>(length(leftForce)-60)) = [];
locs(locs>(length(leftForce)-60)) = [];

if ~isempty(pks)
[Lpks, Llocs, Rpks, Rlocs] = deal(size(pks));
for i = 1:length(pks)
[Lpks(i), Llocs(i)] = max(leftForce(locs(i)-50:locs(i)+50));
[Rpks(i), Rlocs(i)] = max(rightForce(locs(i)-50:locs(i)+50));
Llocs(i) = Llocs(i) + locs(i)-50 - 1;
Rlocs(i) = Rlocs(i) + locs(i)-50 - 1;
end

else
[Lpks, Llocs] = max(leftForce);
[Rpks, Rlocs] = max(rightForce);
end

Pk_ASI = mean(100*(Rpks-Lpks)/mean([Lpks,Rpks],2));

figure(1)
if Sys > 0
subplot(3,1,Sys)
end
plot(time,totalForce,'k',time,leftForce,'g',time,rightForce,'r',...
    time(Llocs),leftForce(Llocs),'go',time(Rlocs),rightForce(Rlocs),'ro')
xlabel('Time (s)')
ylabel('Force (N)')
legend({'Total Force','Left','Right'})
title(plottitle)