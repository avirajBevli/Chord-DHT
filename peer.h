#include "headers.h"
// Node peer_node;
int peer_socket;
struct sockaddr_in server_socket_addr; 

void init_peer(){
	if((peer_socket = socket(AF_INET,SOCK_STREAM,0))==-1){ // create peer_socket
		perror("socket: ");
		exit(-1);
	}
	
	server_socket_addr.sin_family = AF_INET; // IPV4
	server_socket_addr.sin_port = htons(10000); // Port no. of server
	server_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Provide IP address of server 
	bzero(&server_socket_addr.sin_zero,(0));

	if((connect(peer_socket,(struct sockaddr *)&server_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("connect: ");
		close(peer_socket);
		exit(-1);
	}
	
	// cout<<"What is your name: "; fflush(stdout);
	// string client_name; cin>>client_name;
	// // cout<<"client_name: "<<client_name<<endl;
	// send_str(client_name, peer_socket);

	// int client_id;
	// ssize_t bytesRead = recv(peer_socket, &client_id, sizeof(client_id), 0);
	// if(bytesRead<=0){
	// 	cerr<<"Error in getting client_id"<<endl;
	// 	return;
	// }
	// client_node.id = client_id;
	// client_node.socket = peer_socket;
	// client_node.name = client_name;
	// signal(SIGINT, catch_ctrl_c);
}