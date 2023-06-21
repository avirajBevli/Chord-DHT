#include "headers.h"
int listening_socket; 
struct sockaddr_in listening_socket_addr;

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

	signal(SIGINT, catch_ctrl_c);
}