#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

//sample the exponential distribution
double exp_dist(double lambda){
	double x;
	do
	{ 
        x = ((double) rand() / RAND_MAX);
	} while ((x == 0) || (x == 1));

  
  return  (-log(x) * 1/lambda);
  
}


double MAX_TIME = 10e4;

//M/M/1
class QueueA{

    private:
        double l, mu;
        double time;
        double next_arr, next_dep[2];

        bool busy[2];
        int q_len[2];

        double total_queue, total_system,
        total_wait_time, total_net_time;
        int total_q_len[2];

        unsigned int n_departs, n_arrivals;
        unsigned int n_arrivals_q[2];

    public:
        QueueA(double l, double mu):l(l), mu(mu),
        time(0), n_departs(0), n_arrivals(0)
       {
            //for arrival rate of l at each queue
            //net arrival rate at system
            l *= 2.0;

            next_arr = time + inter_arrival();
            for(int i=0; i<2; i++){
                next_dep[i] = MAX_TIME;
                busy[i] = false;
                q_len[i] = 0;
                total_q_len[i] = 0;
                n_arrivals_q[i] = 0;
            }

            total_queue = 0;
            total_system = 0;
            total_wait_time = 0;
            total_net_time = 0;
         
        }

        double inter_arrival(){
            return exp_dist(l);

        }
        double inter_depart(){
            auto x = exp_dist(mu);
            total_net_time += x;
            return x;
        }

        void arrival(){
            n_arrivals++;

            int q_index = get_queue();
            total_q_len[q_index] += q_len[q_index];
            n_arrivals_q[q_index]++;
            if(busy[q_index]){
                q_len[q_index]++;
            }
            else{
                busy[q_index] = true;
                next_dep[q_index] = time + inter_depart();
            }

            next_arr = time + inter_arrival();

        }

        void depart(int q_index){
            n_departs++;

            if(q_len[q_index] > 0){
                next_dep[q_index] = time + inter_depart();
                q_len[q_index]--;
            }
            else{
                next_dep[q_index] = MAX_TIME;
                busy[q_index] = false;
            }
        }
        int get_queue(){
            //equally likely
            return rand()%2;
        }
        int min_next_dep(){
            int min_index =0;
            for(int i=0; i<2; i++){
                if(next_dep[i] < next_dep[min_index])
                    min_index = i;
            }
            return min_index;
        }

        void advance(){
            int min_dep_index = min_next_dep();
            double min_dep = next_dep[min_dep_index];

            double next_event = min(min_dep, next_arr);

            for(int i=0; i<2; i++){
                total_wait_time += q_len[i] * (next_event - time);
                total_net_time += q_len[i] * (next_event - time);
                total_queue += q_len[i];
             
            }

            time = next_event;

            if(next_arr < min_dep)
                arrival();
            else{
                //some dep
                depart(min_dep_index);
            }
        }

        void run(){
            int steps  =0;
            while(time < MAX_TIME){
                advance();
                steps++;
            }
        }
        void print(){
            
            std::cout << "Average no. of workers checked: " << (double)n_departs/MAX_TIME << "\n";
            std::cout << "Average response time: " << total_net_time/n_departs<< "\n";
            std::cout << "Average time for workers until getting checked: " << total_wait_time/n_departs<< "\n";
            std::cout << "Average no. of workers waiting before each officer:\n";
            std::cout << "Queue 1:" << (double)(total_q_len[0])/n_arrivals_q[0] << "\n";
            std::cout << "Queue 2:" << (double)(total_q_len[1])/n_arrivals_q[1] << "\n";
            std::cout << "Net:" << (double)(total_q_len[0] + total_q_len[1])/n_arrivals << "\n";
        }
};

class QueueB{

    private:
        double l, mu;
        double time;
        double next_arr, next_dep[2];
        bool busy[2];
        int q_len;

        double total_queue, total_system,
        total_wait_time, total_net_time;
        unsigned int total_q_len;

        unsigned int n_departs, n_arrivals;

    public:
        QueueB(double l, double mu):l(l), mu(mu),
        time(0), n_departs(0), n_arrivals(0)
       {
            next_arr = time + inter_arrival();
            for(int i=0; i<2; i++){
                next_dep[i] = MAX_TIME;
                busy[i] = false;
            }

            total_queue = 0;
            total_system = 0;
            total_wait_time = 0;
            total_net_time = 0;
            total_q_len = 0;
         
        }

        double inter_arrival(){
            return exp_dist(l);

        }
        double inter_depart(){
            auto x = exp_dist(mu);
            total_net_time += x;
            return x;
        }

        int  get_server(){
            //equally likely
            return rand()%2;
        }

        void arrival(){
            n_arrivals++;

            total_q_len += q_len;

            if(!busy[0]  || !busy[1]){
                if(!busy[0] && !busy[1]){
                    //choose uniformly
                    int s_index = get_server();
                    next_dep[s_index] = time + inter_depart();
                    busy[s_index] = true;
                }
                else if(!busy[0]){
                    next_dep[0] = time + inter_depart();
                    busy[0] = true;
                }
                else{
                    next_dep[1] = time+ inter_depart();
                    busy[1] = true;
                }
            }
            else{
                q_len++;
            }

            next_arr = time + inter_arrival();

        }

        void depart(int s_index){
            n_departs++;

            if(q_len > 0){
                next_dep[s_index] = time + inter_depart();
                q_len--;
            }
            else{
                next_dep[s_index] = MAX_TIME;
                busy[s_index] = false;
            }
        }
   
        int min_next_dep(){
            int min_index =0;
            for(int i=0; i<2; i++){
                if(next_dep[i] < next_dep[min_index])
                    min_index = i;
            }
            return min_index;
        }

        void advance(){
            int min_dep_index = min_next_dep();
            double min_dep = next_dep[min_dep_index];

            double next_event = min(min_dep, next_arr);

            total_wait_time += q_len * (next_event - time);
            total_net_time += q_len* (next_event - time);
            total_queue += q_len;
            

            time = next_event;

            if(next_arr < min_dep)
                arrival();
            else{
                //some dep
                depart(min_dep_index);
            }
        }

        void run(){
            int steps  =0;
            while(time < MAX_TIME){
                advance();
                steps++;
            }
        }
        void print(){
            
            std::cout << "Average no. of workers checked: " << (double)n_departs/MAX_TIME << "\n";
            std::cout << "Average response time: " << total_net_time/n_departs<< "\n";
            std::cout << "Average time for workers until getting checked: " << total_wait_time/n_departs<< "\n";
            std::cout << "Average no. of workers waiting before each officer: " << (double)total_q_len/n_arrivals;
        }
};

//M/M/2/B system
class QueueC{

    private:
        double l, mu;
        double time;
        double next_arr, next_dep[2];

        //regular queue
        double next_arr_reg, next_dep_reg;
        bool busy_reg;
        unsigned int q_len_reg;

        bool busy[2];
        unsigned int q_len[2];
        unsigned int q_size;
        unsigned int n_arrival_reg, n_depart_reg;
        unsigned int total_q_reg;

        double dropping_prob;

        double total_queue, total_system,
        total_wait_time, total_net_time;
        unsigned int total_q_len[2];

        unsigned int n_departs, n_arrivals;

    public:
        QueueC(double l, double mu, int q_size, double dropping_prob = 0.02):l(l), mu(mu), q_size(q_size),
        time(0), n_departs(0), n_arrivals(0), dropping_prob(dropping_prob)
       {
            l *= 2.0;

            next_arr = time + inter_arrival();
            for(int i=0; i<2; i++){
                next_dep[i] = MAX_TIME;
                busy[i] = false;
                q_len[i] = 0;
                total_q_len[i] = 0;
            }

            total_queue = 0;
            total_system = 0;
            total_wait_time = 0;
            total_net_time = 0;

            //reg queue
            next_arr_reg = time + inter_arrival_reg();
            next_dep_reg = MAX_TIME;
            q_len_reg = 0;
            busy_reg = false;
            n_arrival_reg=0;
            n_depart_reg = 0;
            total_q_reg = 0;
        }

        double inter_arrival(){
            return exp_dist(l);

        }
        double inter_arrival_reg(){
            return exp_dist(l/2);
        }
        double inter_depart(){
            auto x = exp_dist(mu);
            total_net_time += x;
            return x;
        }

        void arrival(){
            n_arrivals++;
            
            //get random num
            if(q_len[0] <= q_size && q_len[1] <=  q_size){
                //choose uniformly
                int q_index = get_queue();
                total_q_len[q_index] += q_len[q_index];

                if(!busy[q_index]){
                    //serv immediately
                    next_dep[q_index] = time + inter_depart();
                    busy[q_index] = true;
                }
                else{
                    q_len[q_index]++;
                }
                next_arr = time + inter_arrival();;
            }
            else if(q_len[0] <= q_size){
                total_q_len[0] += q_len[0];
                if(!busy[0]){
                    //serv immediately
                    next_dep[0] = time + inter_depart();
                    busy[0] = true;                }
                else{
                    q_len[0]++;
                }
            }
            else if(q_len[1] <= q_size){
                total_q_len[0] += q_len[0];
                if(!busy[1]){
                    //serv immediately
                    next_dep[1] = time + inter_depart();
                    busy[1] = true;                }
                else{
                    q_len[1]++;
                }   
            }
            else{
                //drop the customer
                double r = (double) rand()/RAND_MAX;
                if(r < 0.02){
                    //person to regular queue
                    arrival_reg();
                }
                else{
                    //drop
                }

            }

            next_arr = time + inter_arrival();

        }
        
        void arrival_reg(){
            n_arrival_reg++;
            total_q_reg += q_len_reg;

            if(!busy_reg){
                next_dep_reg = time + inter_depart();
                busy_reg = true;        
            }
            else{
                q_len_reg++;
            }
            next_arr_reg = time + inter_arrival_reg();
     
        }

        void depart(int q_index){
            n_departs++;

            if(q_len[q_index] > 0){
                next_dep[q_index] = time + inter_depart();
                q_len[q_index]--;
            }
            else{
                next_dep[q_index] = MAX_TIME;
                busy[q_index] = false;
            }
        }
        void depart_reg(){
            n_depart_reg++;
            if(q_len_reg > 0){
                next_dep_reg = time + inter_depart();
                q_len_reg--;
            }
            else{
                next_dep_reg = MAX_TIME;
                busy_reg = false;
            }
        }
        int get_queue(){
            //equally likely
            return rand()%2;
        }

        int min_next_dep(){
            int min_index =0;
            for(int i=0; i<2; i++){
                if(next_dep[i] < next_dep[min_index])
                    min_index = i;
            }
            return min_index;
        }

        void advance(){
            int min_dep_index = min_next_dep();
            double min_dep = next_dep[min_dep_index];

            double next_event = min(min_dep, min(next_arr, min(next_arr_reg, next_dep_reg)));

            for(int i=0; i<2; i++){
                total_wait_time += q_len[i] * (next_event - time);
                total_net_time += q_len[i] * (next_event - time);
                total_queue += q_len[i];
            }
            total_wait_time += q_len_reg* (next_event - time);
            total_net_time += q_len_reg* (next_event - time);

            time = next_event;

            if(next_event == next_arr_reg){
                arrival_reg();
            }
            else if(next_event == next_dep_reg){
                depart_reg();
            }
            else if(next_event == next_arr)
                arrival();
            else{
                //some dep
                depart(min_dep_index);
            }
        }

        void run(){
            int steps  =0;
            while(time < MAX_TIME){
                advance();
                steps++;
            }
        }
        void print(){
            
            std::cout << "Average no. of workers checked: " << (double)(n_departs + n_depart_reg)/MAX_TIME << "\n";
            std::cout << "Average response time: " << total_net_time/n_departs<< "\n";
            std::cout << "Average time for workers until getting checked: " << total_wait_time/n_departs<< "\n";
            std::cout << "Average no. of workers waiting before each officer:\n";
            std::cout << "Queue 1:" << (double)(total_q_len[0])/n_arrivals << "\n";
            std::cout << "Queue 2:" << (double)(total_q_len[1])/n_arrivals << "\n";
            std::cout << "Regular Queue:" << (double)total_q_reg/n_arrival_reg << "\n";
            std::cout << "Net:" << (double)(total_q_len[0] + total_q_len[1] + total_q_reg)/(n_arrivals+n_arrival_reg) << "\n";        }
};

int main(){
    srand(time(NULL));
    std::cout << "Assignment 2: Queue Simulation\n";
    
    int op = -1;
    while(op != 5){
        std::cout << "\n\n1. Case A: 2 Queues, 2 Servers\n";
        std::cout << "2. Case B: 1 Queue, 2 Servers\n";
        std::cout << "3. Case C: 2 Queues of size 5, 2 Servers with a regular queue\n";
        std::cout << "4. Set max time for simulation (Currently " << MAX_TIME  << " units of time)\n";
        std::cout << "5. Quit\n";
        std::cout << "Choice:";
        std::cin >> op;

        double l, mu;
       
        if(op>0 && op<4){
            std::cout << "\n\nEnter Lambda(arrival rate in arrivals/hr):";
            std::cin >> l;
            std::cout << "Enter Mu(service rate in services/hr):";
            std::cin >> mu;
        }
        std::cout << "\n";
        if(l == 0 || mu == 0.0){
            std::cout << "Parameters can not be 0!\n";
            continue;
        }
        switch(op){ 
            case 1:
            {
                if(l >= mu){
                    std::cout << "WARNING::Parameters Lambda and Mu form an UNSTABLE queue system!\n\n" ;
                }
                QueueA q(l, mu);
                q.run();
                q.print();
            }
            break;
            case 2:
            {
                if(l >= 2*mu){
                    std::cout << "WARNING::Parameters Lambda and Mu form an UNSTABLE queue system!\n\n" ;
                }

                QueueB q(l, mu);
                q.run();
                q.print();

            }
            break;
            case 3:
            {
                if(l >= mu){
                    std::cout << "Regular Queue is UNSTABLE with given parameters!\n\n";
                }
                QueueC q(l, mu, 5);
                q.run();
                q.print();
            }
            break;

            case 4:
                do{
                    std::cout << "Enter new max simulation time:";                
                    std::cin >> MAX_TIME;
                    if(MAX_TIME == 0)std::cout << "Invalid Time!\n\n";
                }while(MAX_TIME == 0);
            break;
            case 5:
            break;
            default:
                std::cout << "Invalid Choice";
            break;

        }
    }

 
    return 0;
}