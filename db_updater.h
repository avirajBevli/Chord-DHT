#include "headers.h"
int socket_with_tracker; 
struct sockaddr_in tracker_socket_addr;  // tracker side socket address
// int socket_with_sending_peer; // to receive keys from the peer who is sending entries
// int listening_socket;
// struct sockaddr_in listening_socket_addr; // socket address at which this node listens (used when peer behaves as server)
// struct sockaddr_in sending_peer_socket_addr;  // sending peer side socket address (when connected to current peer)
// // struct sockaddr_in peer_addr_with_tracker; // peer side socket address (when connected to tracker)
// typedef pair<string,string> pair_string_string;


void init_db_updater(){
	if((socket_with_tracker = socket(AF_INET,SOCK_STREAM,0))==-1){ // create socket_with_tracker
		perror("socket: ");
		exit(-1);
	}
	tracker_socket_addr.sin_family = AF_INET; // IPV4
	tracker_socket_addr.sin_port = htons(999); // Port no. of server
	tracker_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Provide IP address of server 
	bzero(&tracker_socket_addr.sin_zero,(0));
	if((connect(socket_with_tracker,(struct sockaddr *)&tracker_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("connect: ");
		close(socket_with_tracker);
		exit(-1);
	}
	cout<<"Connected to tracker"<<endl; fflush(stdout);
	// int db_updater_flag = 0; // let tracker know that the connection is from a new peer not db_updater
	// send(socket_with_tracker, &db_updater_flag, sizeof(db_updater_flag), 0);
}