#include "headers.h"
int peer_listening_socket; 
struct sockaddr_in peer_listening_socket_addr;

int db_updater_listening_socket; 
struct sockaddr_in db_updater_listening_socket_addr;

int db_updater_socket;
struct sockaddr_in db_updater_socket_socket_addr;
set<Node> active_nodes;

void init_tracker(){
	if((peer_listening_socket = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket: ");
		exit(-1);
	}
	peer_listening_socket_addr.sin_family = AF_INET;
	peer_listening_socket_addr.sin_port = htons(BASE_PORT); // port address at which tracker will listen
	peer_listening_socket_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&peer_listening_socket_addr.sin_zero,0);
	if((bind(peer_listening_socket,(struct sockaddr *)&peer_listening_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("bind error: ");
		exit(-1);
	}
	if((listen(peer_listening_socket,8))==-1){
		perror("listen error: ");
		exit(-1);
	}
	// cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Tracker up and running ======   "<<endl<<reset_col;
	// signal(SIGINT, catch_ctrl_c); // TODO
	if((db_updater_listening_socket = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket: ");
		exit(-1);
	}
	db_updater_listening_socket_addr.sin_family = AF_INET;
	db_updater_listening_socket_addr.sin_port = htons(999); // port address at which tracker will listen for DB updation queries
	db_updater_listening_socket_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&db_updater_listening_socket_addr.sin_zero,0);
	if((bind(db_updater_listening_socket,(struct sockaddr *)&db_updater_listening_socket_addr,sizeof(struct sockaddr_in)))==-1){
		perror("bind error: ");
		exit(-1);
	}
	if((listen(db_updater_listening_socket,8))==-1){
		perror("listen error: ");
		exit(-1);
	}
	cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Tracker up and running ======   "<<endl<<reset_col;
}

void handle_peer(int peer_socket_id, struct sockaddr_in peer_socket_addr, int peer_index){
	cout<<"Connected to peer with index:"<<peer_index<<endl;
	send(peer_socket_id, &peer_index, sizeof(peer_index), 0); // send the new peer the peer_index it has been assigned by the tracker

	// receive get_next_peer(new peer id) request by new node
	// send next peer index
	if(active_nodes.empty()){ // the new peer is the first node, hence no need to share any keyvals!
		int min1 = -1;
		send(peer_socket_id, &min1, sizeof(min1), 0);
	}
	else{
		Node tempnode(find_hash(to_string(peer_index)));
		auto it = active_nodes.lower_bound(tempnode);
		if(it==active_nodes.end()){
			it = active_nodes.begin();
		}	
		int next_peer_index = it->node_index;
		send(peer_socket_id, &(next_peer_index), sizeof(next_peer_index), 0); // send the node index as an integer
	}

	active_nodes.insert(Node(peer_index, peer_socket_id));	
}

void listen_to_db_updater(){
	unsigned int len = sizeof(sockaddr_in);
	if((db_updater_socket = accept(db_updater_listening_socket,(struct sockaddr *)&db_updater_socket_socket_addr,&len))==-1){
		perror("accept error: ");
		exit(-1);
	}
	cout<<"Connected to DB updater node!"<<endl; fflush(stdout);

	while(1){
		string key = recv_str(db_updater_socket);
		//cout<<"key:"<<key<<endl;
		string val = recv_str(db_updater_socket);
		//cout<<"val:"<<val<<endl;
		if(key=="#NULL"){
			break;
		}

		// find node for the key
		if(active_nodes.empty()){
			cout<<"Network does not have any peer yet. Discarding keyval pair!"<<endl;
			continue;
		}
		string key_hash = find_hash(key);
		Node tempnode(key_hash);
		auto it = active_nodes.lower_bound(tempnode);
		if(it==active_nodes.end()){ // first node gets the key
			it = active_nodes.begin();
		}
		int key_holder_node_socket_id = (it->node_socket_id);
		int key_holder_node_index = (it->node_index);

		// send key to key_holder_node_index node
		send_str(key, key_holder_node_socket_id); 
		send_str(val, key_holder_node_socket_id); 
		cout<<"key:"<<key<<",val:"<<val<<" with hash: "<<find_hash(key)<<" sent to node index:"<<key_holder_node_index<<endl;
	}
	cout<<"Disconnected from db updater"<<endl;
}