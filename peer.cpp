#include "peer.h"

int main(){
	init_peer_as_client(); // peer joins network. Behaves as client (to tracker and its next peer(from which it gets entries))
	init_peer_as_server(); // open up the listening socket to listen to new peers joining the network
	
	thread t_db(listen_to_tracker_for_new_keyval_entry); // thread to listen to tracker and add new entries to local DB
	t_db.detach();

	while(1){
		int client_socket;
		struct sockaddr_in socket_addr;
		unsigned int len = sizeof(sockaddr_in);
		if((client_socket = accept(listening_socket,(struct sockaddr *)&socket_addr,&len))==-1){
			perror("accept error: ");
			exit(-1);
		}
		cout<<"accepted connection request from "<<ntohs(socket_addr.sin_port)<<endl;
		{
			// thread t(handle_client, client_id, client_socket, socket_addr);
			// t.detach(); // TODO: multithreading
			// lock_guard<mutex> lock(mtx_tracker_socket);  // Acquire the lock using std::lock_guard
			handle_new_peer(client_socket, socket_addr);
		}
	}
	close(listening_socket);
}