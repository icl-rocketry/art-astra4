clear 

% Variables 

% Importing data into an array 
data = readmatrix("astra2_full_testdata.txt");
alldata = []; % empty array of all data - will be filled up later on.
launchData = []; % the first value of this array is defined as the initlaunchtime. 

% check correct import of data
figure
plot(data(:, 1)-data(1,1), data(:, 2))
figure
plot(data(:, 1)-data(1,1), atmospalt(data(:, 2)))


% Variables
refpressure = 101728.25;
maxthrustaltitude = 220;
ag1 = 300; % Apogee guess 1.
ag2 = 350; % Apogee guess 2. 
min_triggeralt = 10;

for i = 1: length(data) % Taking in data point by point - to mimic the pressure readings. 
    alldata(i,1) = data(i,1); % Adding each time datapoint onto the end of all data array. 
    alldata(i,2) = data(i,2); % pressure point.
    alldata(i,3) = 44330 * ( 1 - (alldata(i,2)/refpressure)^(1/5.255)); % calculating the altitude at each pressure point. 
    
    % checking if the altitude is above 10m. 
    if alldata(i,3) > min_triggeralt
        val = find(alldata(:,3)>min_triggeralt); % finding out at which points the altitude is more than 10m.
        launchData = [launchData; alldata(val(end),1), alldata(val(end),3)];
        
        % checking if the max thrust altitude has been reached.
        % the time at which this happens is called MTAT.
        % Index for that time is called MTI.
        if alldata(i,3) > maxthrustaltitude
            MTI = find(launchData(:,2)>maxthrustaltitude,1);
            MTAT = launchData(MTI,1);

            if length(launchData(MTI:end,1)) < 3
                continue
            end


            Ts = launchData(MTI:end,1) - launchData(1,1);
            As = launchData(MTI:end,2);
               
            y = ([sum(Ts.^4), sum(Ts.^3), sum(Ts.^2); sum(Ts.^3), sum(Ts.^2), sum(Ts.^1); sum(Ts.^2), sum(Ts), length(Ts)]);
            z = [sum(As.*(Ts.^2)); sum(As.*(Ts.^1)); sum(As)];                
            coeffs = y\z;
            velcoeffs = [2*coeffs(1); coeffs(2)];
            
            if As(end) == As(end-1)
                continue
            end
            
            ag1 = ag2;
            tguess = -velcoeffs(2)/velcoeffs(1);
            ag2 = coeffs(1)*tguess^2 + coeffs(2)*tguess + coeffs(3);

            
            if ag2 - ag1 < 1 && launchData(end, 2) <= ag2
                vel = velcoeffs(1)*Ts(end) + velcoeffs(2);
                sprintf('Apogee detected at t = %.1f, alt = %.1f, v = %.2f', Ts(end)/1000, As(end), vel)
            end
        end 
    end   
end


    



    

    







%%

% Functions.