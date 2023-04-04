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
        for j = 1: length(val)
            % creating a new lauch data array - for all datapoints where
            % altitude > 10m.
            launchData(j,1) = alldata(val(j),1); % time. 
            launchData(j,2) = alldata(val(j),3); % altitude.
        end
        
        % checking if the max thrust altitude has been reached.
        % the time at which this happens is called MTAT.
        % Index for that time is called MTI.
        if alldata(i,3) > maxthrustaltitude
            MTI = find(launchData(:,2)>maxthrustaltitude,1);
            MTAT = launchData(MTI,1);
    
            while (ag2 - ag1) > 10
                ag1 = ag2 ;
                Ts = launchData(MTI:end,1) - launchData(1,1);
                As = launchData(MTI:end,2);
               
                % Matrix multiplication. 
%                 y = ([sum(Ts.^4), sum(Ts.^3), sum(Ts.^2); sum(Ts.^3), sum(Ts.^2), sum(Ts.^1); sum(Ts.^2), sum(Ts), 1]);
%                 z = [sum(As.*(Ts.^3)); sum(As.*(Ts.^2)); sum(As.*Ts)];                
%                 result = (y.^-1) * z;

                y = ([1, sum(Ts), sum(Ts.^2); sum(Ts.^1), sum(Ts.^2), sum(Ts.^3); sum(Ts.^2), sum(Ts.^3), sum(Ts.^4)]);
                z = [sum(As.*(Ts.^1)); sum(As.*(Ts.^2)); sum(As.*(Ts.^3))];
                %z = [sum(As); sum(As.*(Ts)); sum(As.*Ts.^2)];                
                result = (y.^-1) * z;

                diff = polyder(result);
                tguess = -diff(2)/(diff(1));
                ag2 = polyval(result,tguess);
            end 
        end 

    end
end

    



    

    







%%

% Functions.