#include "headers.h"
int peer_index;
int socket_with_tracker; 
struct sockaddr_in tracker_socket_addr;  // tracker side socket address
int socket_with_sending_peer; // to receive keys from the peer who is sending entries
int listening_socket;
struct sockaddr_in listening_socket_addr; // socket address at which this node listens (used when peer behaves as server)
struct sockaddr_in sending_peer_socket_addr;  // sending peer side socket address (when connected to current peer)
// struct sockaddr_in peer_addr_with_tracker; // peer side socket address (when connected to tracker)
typedef pair<string,string> pair_string_string;

mutex mtx_tracker_socket; // ensure that only one thread is listening at the tracker socket at a time

// thread t_send_messages;
map<pair_string_string,string> keyval_db; // < <hash(key),key>, val>


// open listening socket
void init_peer_as_server(){
	if((listening_socket = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket: ");
		exit(-1);
	}
	listening_socket_addr.sin_family = AF_INET;
	listening_socket_addr.sin_port = htons(BASE_PORT+peer_index); // port address at which peer will listen to new peers' request
	listening_socket_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&listening_socket_addr.sin_zero,0);
	if((bind(listening_socket,(struct sockaddr *)&listening_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("bind error: ");
		exit(-1);
	}
	if((listen(listening_socket,8))==-1){
		perror("listen error: ");
		exit(-1);
	}
	cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Peer ready to serve requests ======   "<<endl<<reset_col;
	// signal(SIGINT, catch_ctrl_c); // TODO
}

// establish socket connection with tracker
void init_peer_as_client(){
	if((socket_with_tracker = socket(AF_INET,SOCK_STREAM,0))==-1){ // create socket_with_tracker
		perror("socket: ");
		exit(-1);
	}
	tracker_socket_addr.sin_family = AF_INET; // IPV4
	tracker_socket_addr.sin_port = htons(BASE_PORT); // Port no. of server
	tracker_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Provide IP address of server 
	bzero(&tracker_socket_addr.sin_zero,(0));
	if((connect(socket_with_tracker,(struct sockaddr *)&tracker_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("connect: ");
		close(socket_with_tracker);
		exit(-1);
	}
	// int new_peer_flag = 1; // let tracker know that the connection is from a new peer not db_updater
	// send(socket_with_tracker, &new_peer_flag, sizeof(new_peer_flag), 0); 
    recv(socket_with_tracker, &peer_index, sizeof(peer_index), 0);
    cout<<"peer_index:"<<peer_index<<endl;
    cout<<"peer hash: "<<find_hash(to_string(peer_index))<<endl;
  	int sending_peer_index;
  	recv(socket_with_tracker, &sending_peer_index, sizeof(sending_peer_index), 0);
  	cout<<"sending_peer_index:"<<sending_peer_index<<endl;

	// contact the next peer
	// // Get the local address of the client socket
	// socklen_t len = sizeof(peer_addr_with_tracker);
	// if (getsockname(peer_socket, (struct sockaddr*)&peer_addr_with_tracker, &len) == -1) {
	//  perror("getsockname");
	//  return;
	// }
	// // Extract the IP address and port number
	// char ip[INET_ADDRSTRLEN];
	// inet_ntop(AF_INET, &(peer_addr_with_tracker.sin_addr), ip, INET_ADDRSTRLEN);
	// uint16_t port = ntohs(peer_addr_with_tracker.sin_port);
	// // cout << "Client: " << ip << ", " << port << endl;

    // string ipstr(ip);
    // peernode.nodeIP = ipstr;
    // peernode.nodePort = to_string(port);
    // peernode.nodeID = find_hash(peernode.nodeIP + peernode.nodePort);
    // peernode.socket_id = socket_with_tracker;
    // peernode.socket_addr_with_tracker = peer_addr_with_tracker;
    // peernode.listening_socket_addr = ; // TODO: set later in code
    // send get_next_peer(id) request to tracker
    // send_str(socket_with_tracker, peernode.nodeID);
	

	// open up a socket with next peer (sending peer) {if it exists}
  	if(sending_peer_index == -1){
  		cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Peer initialised as first client ======   "<<endl<<reset_col;
  		return;
  	}
	if((socket_with_sending_peer = socket(AF_INET,SOCK_STREAM,0))==-1){ 
		perror("socket: ");
		exit(-1);
	}
	sending_peer_socket_addr.sin_family = AF_INET; // IPV4
	sending_peer_socket_addr.sin_port = htons(BASE_PORT + sending_peer_index); // Port no. of sending peer (acting as a server)
	sending_peer_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	bzero(&sending_peer_socket_addr.sin_zero,(0));
	if((connect(socket_with_sending_peer,(struct sockaddr *)&sending_peer_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("connect:: ");
		close(socket_with_sending_peer);
		exit(-1);
	}
	send(socket_with_sending_peer, &peer_index, sizeof(peer_index), 0); // send peer_index of this node to new node so that new node can decide which entries to send the this node
	// cout<<"sent peer_index:"<<peer_index<<endl;

	// use this socket to recv DB entries from the sending peer and update current peer's own data
	int numentries_to_recv;
	recv(socket_with_sending_peer, &numentries_to_recv, sizeof(numentries_to_recv), 0);
	cout<<"numentries_to_recv:"<<numentries_to_recv<<endl;
	for(int i=0;i<3*numentries_to_recv;i++){
		string key_hash = recv_str(socket_with_sending_peer);
		string key = recv_str(socket_with_sending_peer);
		string val = recv_str(socket_with_sending_peer);
		keyval_db[make_pair(key_hash,key)] = val;
	}

	cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Peer initialised as client ======   "<<endl<<reset_col;
}	

// send entries to the new peer that has arrived on the network
void handle_new_peer(int client_socket, struct sockaddr_in socket_addr){
	int new_peer_index;
	recv(client_socket, &new_peer_index, sizeof(new_peer_index), 0); // new_peer_index of the new peer
	// cout<<"received request from new peer with index:"<<new_peer_index<<endl;

	// go through the keyval_db of this node and send requied entries to new peer
	vector< pair<pair_string_string, string> > entries_to_send;
	string new_peer_hash_id = find_hash(to_string(new_peer_index));
	for(auto &m:keyval_db){
		if(m.first.first < new_peer_hash_id){
			entries_to_send.push_back(make_pair(make_pair(m.first.first,m.first.second), m.second));
		}
	}
	int numentries_to_send = entries_to_send.size();
	send(client_socket, &numentries_to_send, sizeof(numentries_to_send), 0);
	cout<<"sent numentries_to_send: "<<numentries_to_send<<endl;

	// send entries to the new node
	for(int i=0;i<numentries_to_send;i++){
		send_str(entries_to_send[i].first.first, client_socket);
		send_str(entries_to_send[i].first.second, client_socket);
		send_str(entries_to_send[i].second, client_socket);
	}

	// update DB of current node
	for(int i=0;i<numentries_to_send;i++){
		keyval_db.erase(keyval_db.find(make_pair(entries_to_send[i].first.first, entries_to_send[i].first.second)));
	}	
	entries_to_send.clear();
	cout<<"Handled new peer "<<new_peer_index<<endl;
}

void listen_to_tracker_for_new_keyval_entry(){
	while(1){
		//lock_guard<mutex> lock(mtx_tracker_socket);

		string key = recv_str(socket_with_tracker);
		if(key=="#NULL"){
			break;
		}
		string val = recv_str(socket_with_tracker);
		string key_hash = find_hash(key);
		cout<<"recevied key:"<<key<<", val:"<<val<<" to store"<<endl;
		keyval_db[make_pair(key_hash,key)] = val;
	}
	cout<<"stopped listening to tracker for new keyval entry"<<endl;
}