clear 
close all
clc
cols = ["#FAC05E", "#59CD90", "#EE6352", "#3FA7D6"];

% Variables 

% Importing data into an array 
data = readmatrix("astra2_full_testdata.txt");
availabledata = []; % empty array of all data - will be filled up later on.
launchData = []; % the first value of this array is defined as the initlaunchtime. 

% % check correct import of data
% figure
% plot(data(:, 1)-data(1,1), data(:, 2))
% figure
% plot(data(:, 1)-data(1,1), atmospalt(data(:, 2)))

% Variables
refpressure = 101728.25;
maxthrustaltitude = 220;
ag1 = 300; % Apogee guess 1.
ag2 = 350; % Apogee guess 2. 
min_triggeralt = 10;
flighttime = 45; % after motor burnout

tic
% frame rate of animation
% recommended to keep at 24 - any lower and it doesn't look smooth, higher
% looks smoother but takes longer to process
fr = 20;
fs = 20;
lw = 2;
ms = 14;
af = figure;
grid minor
hold on
set(af, 'Position', [0 0 2560 1440]);
plot(-10, 0, 'x', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(1))
plot(-10, 0, 'x', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(2))
plot(-10, 0, '-', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(3))
plot(-10, 0, '-', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(4))

xlim([200 400])
ylim([min(atmospalt(data(:, 2)))-10, 600])
ylabel('Altitude (m)')
xlabel('Time (s)')
legend('Prelaunch', 'Actual Trajectory', 'Current Predicted Trajectory', ...
    'Predicted Trajectory at Separation Trigger','AutoUpdate','off', 'FontSize', fs)
caption = text(200, 550, sprintf('Launch not detected, t = 0'));

% v = VideoWriter('apogee_fit_astra2.mp4','MPEG-4');
% v.FrameRate = fr;
% v.Quality = 98;

% open(v);

for i = 5000:6000 % Taking in data point by point - to mimic the pressure readings. 
    availabledata(i,1) = data(i,1); % Adding each time datapoint onto the end of all data array. 
    availabledata(i,2) = data(i,2); % pressure point.
    availabledata(i,3) = 44330 * ( 1 - (availabledata(i,2)/refpressure)^(1/5.255)); % calculating the altitude at each pressure point. 
    
    delete(caption)
    % checking if the altitude is above 10m. 
    % if availabledata(i,3) > min_triggeralt
    val = find(availabledata(:,3)>min_triggeralt); % finding out at which points the altitude is more than 10m.
    if ~isempty(val)
        launchData = [launchData; availabledata(val(end),1), availabledata(val(end),3)];
        
        % checking if the max thrust altitude has been reached.
        % the time at which this happens is called MTAT.
        % Index for that time is called MTI.
        if availabledata(i,3) > maxthrustaltitude
            MTI = find(launchData(:,2)>maxthrustaltitude,1);
            MTAT = launchData(MTI,1);

           if length(launchData(MTI:end,1)) < 3
               drawnow
%                writeVideo(v, getframe(af));
               continue
           end

            Ts = launchData(MTI:end,1) - launchData(1,1);
            As = launchData(MTI:end,2);
            
            % Try to get this section to update, rather than rewriting it. 
            y = ([sum(Ts.^4), sum(Ts.^3), sum(Ts.^2); sum(Ts.^3), sum(Ts.^2), sum(Ts.^1); sum(Ts.^2), sum(Ts), length(Ts)]);
            z = [sum(As.*(Ts.^2)); sum(As.*(Ts.^1)); sum(As)];                
            coeffs = y\z; % shouldnt be singular. 
            velcoeffs = [2*coeffs(1); coeffs(2)];
            
      
            ag1 = ag2;
            tguess = -velcoeffs(2)/velcoeffs(1);
            ag2 = coeffs(1)*tguess^2 + coeffs(2)*tguess + coeffs(3);
            
            anim_xs = [min(Ts):0.05:min(Ts) + flighttime*1000]';
            anim_ys = coeffplot(anim_xs, coeffs);
            
            if exist('trajpred')
                delete(trajpred)
%                 delete(predap)
            end
            trajpred = plot((anim_xs + launchData(1,1))/1000, anim_ys, '-', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(3));
%             predap = text(300, 300, sprintf('Current Apogee prediction %.1f m at t = %.2f', ag2, tguess/1000), 'FontSize', fs);
            if As(end) ~= As(end-1)
                if ag2 - ag1 < 1 && launchData(end, 2) <= ag2 && launchData(end, 1) > tguess
                    vel = velcoeffs(1)*Ts(end) + velcoeffs(2);
                    fprintf('Apogee detected at t = %.1f, alt = %.1f, v = %.2f\n', Ts(end)/1000, ag2, vel)                
                    if ~exist('apogeeflag')
                        plot((anim_xs + launchData(1,1))/1000, anim_ys, '-', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(4));
                        xline((tguess + launchData(1,1))/1000, 'k--', 'LineWidth', lw);
                        yline(ag2, 'k--', 'LineWidth', lw);
                        apogeeflag = text(310, 450, sprintf('Seperation Triggered at alt = %.1f t = %.2f', ag2, (tguess+launchData(1,1))/1000), 'FontSize', fs);
                    end
                end
            end
        end
        plot(availabledata(1:min(val), 1)/1000, availabledata(1:min(val), 3), 'x', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(1))
        plot(availabledata(min(val):end, 1)/1000, availabledata(min(val):end, 3), 'x', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(2))
        caption = text(220, 300, sprintf('Launch detected, t = %.2f', availabledata(end, 1)/1000), 'FontSize', fs);

    else
        plot(availabledata(:, 1)/1000, availabledata(:, 3), 'x', 'LineWidth', lw, 'MarkerSize', ms, 'Color', cols(1))
        caption = text(220, 300, sprintf('Launch not detected, t = %.2f', availabledata(end, 1)/1000), 'FontSize', fs);
    end

    drawnow
    % writeVideo(v, getframe(af));
end

% close(v)


function ys = coeffplot(xs, coeffs)

    ys = zeros(length(xs), 1);

    ys = ys + coeffs(1).*(xs.^2) + coeffs(2).*xs + coeffs(3);

end

    



    

    







%%

% Functions.