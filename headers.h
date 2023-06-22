#include <bits/stdc++.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

#define HASH_LEN 5
#define MAX_LEN 200
#define NUM_COLORS 6
string reset_col = "\033[0m";
string colors[] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m","\033[36m"};
string find_color(int code){
	return colors[code%NUM_COLORS];
}

string find_hash(string str){
	hash<string> sha256;
	string hashval = to_string(sha256(str));
	string ret = hashval.substr(0,HASH_LEN);
	return ret;
}

struct Node{
	int node_index; // the index of node in the network
	string node_hash_id; // nodeID = Hash(to_string(node_index))
	// string nodeIP;
	// string nodePort; // port with which it communicates with tracker
	// int socket_id; // socket_id through which tracker communicates with the node
	// struct sockaddr_in socket_addr_with_tracker; // socket address through which node is connected to tracker 
	// struct sockaddr_in listening_socket_addr; // socket address at which this peer listens

	Node(string node_hash_id_in){
		// nodeIP = "127.0.0.1";
		// nodePort = "1010";
		node_hash_id = node_hash_id_in;
		node_index = 0;
	}

	Node(int node_index_in){
		node_index = node_index_in;
		node_hash_id = find_hash(to_string(node_index));
	}

	bool operator < (const Node &other) const {
		if(node_hash_id < other.node_hash_id) return 1;
		else return 0;
	}
};


string recv_str(int socket_id){
	// Receive an integer value
    int msglen;
    ssize_t bytesRead = recv(socket_id, &msglen, sizeof(msglen), 0);
    if (bytesRead == -1) {
        cout << "Failed to receive the integer value." << endl;
        return "#NULL";
    }
    else if(bytesRead == 0){
    	cout << "read 0 bytes"<<endl;
    	return "#NULL";
    }

    string received_string;
    int msglenrecvd = 0;
    while(msglenrecvd < msglen){
    	char msg_temp[MAX_LEN];
		ssize_t bytesRead = recv(socket_id, msg_temp, sizeof(msg_temp),0);
		if(bytesRead == -1){
			cout << "in while, Failed to receive the integer value." << endl;
        	return "#NULL";
		}
		else if(bytesRead == 0){
	    	cout << "in while, read 0 bytes"<<endl;
	    	return "#NULL";
	    }
		string str_temp(msg_temp);
		received_string += str_temp;
		msglenrecvd+=(str_temp.size());
    }
    // cout<<"msg received: "<<received_string<<"."<<" from "<<socket_id<<endl;
    fflush(stdout);
    return received_string;
}

void send_str(string str, int socket_id){
	// send integer value (length of message)
	int len = str.size();
	send(socket_id, &len, sizeof(len), 0);


	const char* messageData = str.c_str();
	size_t messageLength = str.length();
	size_t totalSent = 0;
    while(totalSent < messageLength) {
        ssize_t bytesSent = send(socket_id, messageData + totalSent, messageLength - totalSent, 0);
        if(bytesSent == -1) {
            cout << "in while, failed to send data." << endl;
            return;
        }
        totalSent += bytesSent;
    }
    // cout<<"msg sent: "<<messageData<<". to "<<socket_id<<endl;
	fflush(stdout);
}
