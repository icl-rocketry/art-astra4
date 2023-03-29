import requests 
import time

# Function to establish a connection to the ESPs. 
def CheckConnection(url):
    # Send the HTTP GET request and get the response
    reply1= requests.get(url)
    return reply1.text

# Function to ask about the health of the ESP.
def CheckHealth(url):
    # Send the HTTP GET request and get the response
    reply2 = requests.get(url)
    return reply2.text

# Ground Station Code.
# Variables:
n = 1 # Number of ESPs. 
# url_go = ["http://{}/go","http://{}/go","http://{}/go"]
# url_health = ["http://{}/health","http://{}/health","http://{}/health","http"]
url_go = ["http://localhost/go"]
url_health = ["http://localhost/health"]
Greenlight = [0,0,0]
Run = True

# Initially, registering a connection with ESPs. 
for i in range(n):
    reply1 = CheckConnection(url_go[i])
    print("Reply 1 is ", reply1)
    # Sending a message to each ESP if a connection has been established. 
    if reply1 != "Yes":
        print("Connection has not been established. There has been a problem in ESP ", i)
    continue 
        
# Checking the health of the ESPs. 
while Run == True:
    for j in range(n):
        reply2 = CheckHealth(url_health[j])
        print("Reply 2 is ", reply2)
        if reply2 == "Healthy":
            Greenlight[j] == 1
            print("ESP ", j, " is healthy")
        else :
            Greenlight[j] == 0
            print("ESP ", j, " is deaded")
    time.sleep(3)
        
    
    




