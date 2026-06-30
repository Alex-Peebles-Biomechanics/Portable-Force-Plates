%% Restart connection
clear, clc, close all

%% Connect to force plates
% PortsAvailable = serialportlist("available");
clear S1

pause(3)

%clear S1
S1 = serialport("COM19",28800);
configureTerminator(S1,"CR/LF");
flush(S1);
write(S1,'0 ',"string")
S1.UserData = struct("t",[],"data1",[],"data2",[]);
configureCallback(S1,"byte",12,@readBinaryData)

figure(1)

%% Callback functions

function readBinaryData(src, ~)

firstVal = typecast(uint8(read(src,4,"uint8")),'single');
secondVal = typecast(uint8(read(src,4,"uint8")),'single');
thirdVal = typecast(uint8(read(src,4,"uint8")),'single');
    if (secondVal == 0 && thirdVal == 0 && firstVal == 0)
        Sys = 0;
        study = num2str(src.UserData.t(1));
        PID = num2str(src.UserData.data1(1));
        movement = src.UserData.data2(1);
        plottitle = ['Sub',PID,', trial: ',num2str(movement)];
        trialname = strcat(study, "_", PID, "_", num2str(movement), ".xlsx");
        outmat = [src.UserData.t(2:end)', src.UserData.data1(2:end)', src.UserData.data2(2:end)'];
        if movement < 3
            header = {'time (ms)','CoPx','CoPy'};
            writecell(header,trialname,'Range','A1:C1')
            writematrix(outmat,trialname,'WriteMode','append');
            [aveVelocity, swayArea] = Analyze_Balance(outmat,plottitle,Sys);
            write(src,['Sway Area: ', num2str(round(swayArea,1)), ' cm2,'],"string")
            write(src,['Mean Vel: ', num2str(round(aveVelocity,1)), ' cm/s,'],"string")
            write(src,',',"string")
            write(src,',',"string")
        elseif movement == 3
            header = {'time (ms)','Left Force','Right Force'};
            writecell(header,trialname,'Range','A1:C1')
            writematrix(outmat,trialname,'WriteMode','append');
            [Imp_ASI, Pk_ASI] = Analyze_Squat(outmat,plottitle,Sys);
            write(src,['Imp Sym: ', num2str(round(Imp_ASI,1)), ' %,'],"string")
            write(src,['Pk Sym: ', num2str(round(Pk_ASI,1)), ' %,'],"string")
            write(src,',',"string")
            write(src,',',"string")
        else 
            header = {'time (ms)','Left Force','Right Force'};
            writecell(header,trialname,'Range','A1:C1')
            writematrix(outmat,trialname,'WriteMode','append');
            write(src,'Jumping analysis,',"string")
            write(src,'coming soon,',"string")
            write(src,',',"string")
            write(src,',',"string")
        end
        src.UserData.t = [];
        src.UserData.data1 = [];
        src.UserData.data2 = [];
    else
        src.UserData.t(end+1) = firstVal;
        src.UserData.data1(end+1) = secondVal;
        src.UserData.data2(end+1) = thirdVal;
    end
end


