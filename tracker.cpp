#include "tracker.h"
vector<int> active_sockets;

int main(){
	init_tracker();
	int peer_index=1;
	while(1){
		int peer_socket;
		struct sockaddr_in peer_socket_addr;
		unsigned int len = sizeof(sockaddr_in);
		if((peer_socket = accept(listening_socket,(struct sockaddr *)&peer_socket_addr,&len))==-1){
			perror("accept error: ");
			exit(-1);
		}
		handle_peer(peer_socket, peer_socket_addr, peer_index);
		// TODO: use threads and mutex lock so that at a time one peer only shares keys with one other peer
		// thread t(handle_peer, peer_socket, peer_socket_addr, peer_index);
		// t.detach();
		active_sockets.push_back(peer_socket);
		peer_index++;
	}
	close(listening_socket);

	// close all socket connections to peers
	for(int i=0;i<active_sockets.size();i++){
		close(active_sockets[i]);
	}
	return 0;	
}
