#include "peer.h"

int main(){
	init_peer_as_client(); // peer joins network. Behaves as client (to tracker and its next peer(from which it gets entries))
	init_peer_as_server(); // open up the listening socket to listen to new peers joining the network
	while(1){
		int client_socket;
		struct sockaddr_in socket_addr;
		unsigned int len = sizeof(sockaddr_in);
		if((client_socket = accept(listening_socket,(struct sockaddr *)&socket_addr,&len))==-1){
			perror("accept error: ");
			exit(-1);
		}
		// thread t(handle_client, client_id, client_socket, socket_addr);
		// t.detach(); // TODO: multithreading
		handle_new_peer(client_socket, socket_addr);
	}
	close(listening_socket);
}