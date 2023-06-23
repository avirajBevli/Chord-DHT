#include "tracker.h"
vector<int> active_sockets;

int main(){
	init_tracker();
	thread t_db(listen_to_db_updater); // thread to interact with databse_updater node
	t_db.detach();

	int peer_index=1;
	while(1){
		int client_socket;
		struct sockaddr_in client_socket_addr;
		unsigned int len = sizeof(sockaddr_in);
		if((client_socket = accept(peer_listening_socket,(struct sockaddr *)&client_socket_addr,&len))==-1){
			perror("accept error: ");
			exit(-1);
		}
		handle_peer(client_socket, client_socket_addr, peer_index);
		// TODO: use threads and mutex lock so that at a time one peer only shares keys with one other peer
		// thread t(handle_peer, peer_socket, client_socket_addr, peer_index);
		// t.detach();
		active_sockets.push_back(client_socket);
		peer_index++;
		// int is_new_peer; recv(client_socket, &is_new_peer, sizeof(is_new_peer), 0);
		// if(is_new_peer == 1){ // connection request from a new peer in the network
		// 	handle_peer(client_socket, client_socket_addr, peer_index);
		// 	// TODO: use threads and mutex lock so that at a time one peer only shares keys with one other peer
		// 	// thread t(handle_peer, peer_socket, client_socket_addr, peer_index);
		// 	// t.detach();
		// 	active_sockets.push_back(client_socket);
		// 	peer_index++;
		// }
		// else{ // connection request from the db_updater node
		// 	handle_db_updater(client_socket, client_socket_addr);
		// }
	}
	close(peer_listening_socket);

	// close all socket connections to peers
	for(int i=0;i<active_sockets.size();i++){
		close(active_sockets[i]);
	}
	return 0;	
}
