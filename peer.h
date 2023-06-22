#include "headers.h"
int peer_index;
int socket_with_tracker; 
struct sockaddr_in tracker_socket_addr;  // tracker side socket address

// struct sockaddr_in sending_peer_socket_addr;  // sending peer side socket address (when connected to current peer)
// struct sockaddr_in peer_addr_with_tracker; // peer side socket address (when connected to tracker)
// struct sockaddr_in listening_socket_addr; // socket address at which this node listens (used when peer behaves as server)
// int socket_with_sending_peer; // to receive keys from the peer who is sending entries
// int listening_socket;

// thread t_send_messages;
// map<string,string> keyval_db;


// establish socket connection with tracker
void init_peer(){
	if((socket_with_tracker = socket(AF_INET,SOCK_STREAM,0))==-1){ // create socket_with_tracker
		perror("socket: ");
		exit(-1);
	}
	tracker_socket_addr.sin_family = AF_INET; // IPV4
	tracker_socket_addr.sin_port = htons(10000); // Port no. of server
	tracker_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Provide IP address of server 
	bzero(&tracker_socket_addr.sin_zero,(0));
	if((connect(socket_with_tracker,(struct sockaddr *)&tracker_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("connect: ");
		close(socket_with_tracker);
		exit(-1);
	}
    recv(socket_with_tracker, &peer_index, sizeof(peer_index), 0);
    send_str(find_hash(to_string(peer_index)), socket_with_tracker);




    // // Get the local address of the client socket
    // socklen_t len = sizeof(peer_addr_with_tracker);
    // if (getsockname(peer_socket, (struct sockaddr*)&peer_addr_with_tracker, &len) == -1) {
    //     perror("getsockname");
    //     return;
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


	// receive next_peer index from tracker
	// int sending_peer_index;
	// recv(socket_with_tracker, &sending_peer_index, sizeof(sending_peer_index), 0);

	

	// // open up a socket with next peer (sending peer)
	// if((socket_with_sending_peer = socket(AF_INET,SOCK_STREAM,0))==-1){ 
	// 	perror("socket: ");
	// 	exit(-1);
	// }
	// sending_peer_socket_addr.sin_family = AF_INET; // IPV4
	// sending_peer_socket_addr.sin_port = htons(10000+sending_peer_index); // Port no. of sending peer (acting as a server)
	// sending_peer_socket_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	// bzero(&sending_peer_socket_addr.sin_zero,(0));
	// if((connect(socket_with_sending_peer,(struct sockaddr *)&sending_peer_socket_addr,sizeof(struct sockaddr_in)))==-1){
	// 	perror("connect: ");
	// 	close(socket_with_sending_peer);
	// 	exit(-1);
	// }

	// // use this socket to recv DB entries from the sending peer
	// send_str(p);
}	

// void peer_as_server_init(){
// 	if((listening_socket = socket(AF_INET,SOCK_STREAM,0))==-1){
// 		perror("socket: ");
// 		exit(-1);
// 	}
// 	listening_socket_addr.sin_family = AF_INET;
// 	listening_socket_addr.sin_port = htons(10000+peer_index); // port address at which peer will listen to new peers' request
// 	listening_socket_addr.sin_addr.s_addr = INADDR_ANY;
// 	bzero(&listening_socket_addr.sin_zero,0);
// 	if((bind(listening_socket,(struct sockaddr *)&listening_socket_addr,sizeof(struct sockaddr_in)))==-1){
// 		perror("bind error: ");
// 		exit(-1);
// 	}
// 	if((listen(listening_socket,8))==-1){
// 		perror("listen error: ");
// 		exit(-1);
// 	}
// 	cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Peer ready to server requests ======   "<<endl<<reset_col;
// 	// signal(SIGINT, catch_ctrl_c); // TODO
// }

// // send required database entries to the node that is demanding them
// void send_entries(){
// 	// receive listening_socket_address of node who wants entries

// 	// find iterator

// 	open_socket();

// 	// send entries

// 	// update own database
// }
