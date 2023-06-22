#include "headers.h"
int listening_socket; 
struct sockaddr_in listening_socket_addr;
set<Node> active_nodes;

void init_tracker(){
	if((listening_socket = socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("socket: ");
		exit(-1);
	}
	listening_socket_addr.sin_family = AF_INET;
	listening_socket_addr.sin_port = htons(10000); // port address at which tracker will listen
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
	cout<<colors[NUM_COLORS-1]<<"\n\t  ====== Tracker up and running ======   "<<endl<<reset_col;
	// signal(SIGINT, catch_ctrl_c); // TODO
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

	active_nodes.insert(Node(peer_index));	
}
