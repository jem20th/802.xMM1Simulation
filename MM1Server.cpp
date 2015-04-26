//ECS152A Project
//Harrison Ching

#include <iostream>
#include <queue>
#include <list>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#define MAXBUFFER 1

using namespace std;

struct Packet {
	double service_time;
};
struct Event {
	double event_time;
	int ifArrival;
	Packet *data;
};

double negativeExpDist(double rate) {
	double u;
	u = drand48();
	return ((-1/rate)*log(1-u));
} 

bool my_sort(Event *prev, Event *next) {
	double ex1 = 0, ex2 = 0;
	ex1 = prev->event_time;
	ex2 = next->event_time;
	return(ex1 < ex2);
}

int main(){
	
	//Initi
	queue<Packet*> mm1;
	list<Event*> GEL;
	int length = 0;	//number of packets in queue
	int dropped_packets = 0;
	int i;
	double time = 0;	//current time
	double total_length = 0;	//total number of packets
	double mean_length = 0;
	double server_util = 0;
	double busy_server = 0;
	double lamda;
	double mu = 1;
	
	//Input for plot
	cout << "Lamda: ";
	cin >> lamda;
//	cout << "MAXBUFFER: ";
//	cin >> MAXBUFFER;
	
	//create first arrival event and insert into GEL
	Packet *ichi = new Packet;
	ichi->service_time = negativeExpDist(mu);
	Event *first = new Event;
	first->ifArrival = 1;
	first->event_time = negativeExpDist(lamda);
	first->data = ichi;
	GEL.push_back(first);
	
	//Event Processing
	for (i = 0; i < 100000; i++) {
		//1. get the first event from the GEL;
		Event *fromGEL = GEL.front();
		GEL.pop_front();
		//2. If the event is an arrival then process-arrival-event;
		if(fromGEL->ifArrival == 1)
		{
			//Set current time to event time
			time = fromGEL->event_time;
			//Create NEXT arrival event
			Event *nextEvent = new Event;
			nextEvent->ifArrival = 1;
			//Time of next arrival
			Packet *nextPacket = new Packet;
			nextPacket->service_time = negativeExpDist(mu);
			//Find service time
			nextEvent->event_time = time + negativeExpDist(lamda);
			nextEvent->data = nextPacket;
			//Insert event into GEL
			GEL.push_back(nextEvent);
			GEL.sort(my_sort);
			//Increment total length
			total_length += length;
			
			//a) If the server is free, transmit packet
			if(length == 0) {
				//1. Get service time (using a new packet)	
				Packet *transPacket = new Packet;
				transPacket = fromGEL->data;
				//2. Create departure event
				Event *transEvent = new Event;
				transEvent->ifArrival = 0;
				transEvent->event_time = time + transPacket->service_time;
				transEvent->data = transPacket;
				//3. Insert event into GEL
				GEL.push_back(transEvent);
				GEL.sort(my_sort);
				//Increment queue length
				length++;
			}
			//if server is busy
			else {
				//if queue is not full
				if((length-1) < MAXBUFFER){
					//put packet in queue
                    mm1.push(fromGEL->data);
					//Increment queue length (update statistics)
                    length++; 
                }
				//if queue is full, drop the packet
                else {
                    dropped_packets++;
				}
			}
		}
		//3. Otherwise it must be a departure event and hence process-service-completion;
		else {
			//set current time 
			time = fromGEL->event_time;
			//update statistics
			busy_server += fromGEL->data->service_time;
			//decrement length since it's a packet departure
			length--;
			if(length == 0) {
				//nothing
			}
			else{
				//1. dequeue first packet from mm1
				Packet *dePacket = new Packet;
				dePacket = mm1.front();
				mm1.pop();
				//2. create a new departure event (current time + transmit time)
				Event *deEvent = new Event;
				deEvent->ifArrival = 0;
				deEvent->event_time = time + dePacket->service_time;
				deEvent->data = dePacket;
				//3. Insert event into GEL
				GEL.push_back(deEvent);
				GEL.sort(my_sort);
			}
		}
	}
	
	//Output Statistics
	mean_length = total_length/time;
	server_util = busy_server/time;
	cout << "Mean Queue Length: " << mean_length << endl;
	cout << "Mean Server Busy Time: " << server_util << endl;
	cout << "Number of Packets Dropped: " << dropped_packets << endl;

	return 0;
}
