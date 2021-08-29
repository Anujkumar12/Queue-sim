# Queue Simulation

### Usage
Using the Makefile

make

OR

g++ main.cpp -o main

Now the executable file can be run as:
./main


### Options given to the end-user: 
```
1. Case A: 2 Queues, 2 Servers
2. Case B: 1 Queue, 2 Servers
3. Case C: 2 Queues of size 5, 2 Servers
4. Quit 
(There is only one condition: Arraival rate should be less than total Service rate.)
```
## Output
```
1. The average number of workers getting checked?
2. Average response time for workers in getting checked?
3. Average time for which a worker has to wait until getting checked?
4. The average number of workers waiting in the queue before each officer?
```
## Approach Used
### Case A: 2 Queues, 2 Servers
   Two seperate queues and two diffrent servers are given. It is an example of M/M/1 queue. 
   We are executing (simulating) the process for a fixed time 10e4 and calculating above mentioned parameters.
   
   Code Structure: Created a class QueueA which contains different properties i.e. Queue length, Next Arrival and Next Deprature time.
   Execute the next arrival or next departure process whichever has minimum time. When processing also calculate the required parameters till that particular time. 
   Next arrival and Next departure time would be incremented in Exponential distribution manner.
   Here we are using rand() function for assigning a new worker to any of the two officers in order to implement 50% probability condition.  
### Case B: 1 Queue, 2 Servers
   In this case a single queue and two diffrent servers are given. It is an example of M/M/2 queue. 
   We are executing (simulating) the process for a fixed time 10e4 and calculating above mentioned parameters.
   
   Code Structure: Created a class QueueB which contains different properties i.e. Queue length, Next Arrival and Next Deprature time.
   Execute the next arrival or next departure process whichever has minimum time. When processing also calculate the required parameters till that particular time. 
   Next arrival and Next departure time would be incremented in Exponential distribution manner.
   Here we are using rand() function for queue selection whenever both the servers are idle or assign worker to the other server if one of them is already executing a process. In case both the servers are busy we will simply increment the queue length.
### Case C: 2 Queues of size 5, 2 Servers with regular Queue
   Case C is similar to Case 1. We are given queues of limited size(5). 
   Functionality would be exaclty same except workers will be dropped once queue length exceeds the given capacity. We will compare queue length of selected queue when processing, if found less than the maximum capacity, worker will be assigned to the respective queue else drop that particular worker with probablity 0.98. 

   With probability 0.02, dropped worker is assigned to a regular queue, which is also simulated.
