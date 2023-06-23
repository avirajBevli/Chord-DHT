#include "db_updater.h"

int main(){
	init_db_updater();
	while(1){
		string key; cin>>key; // cout<<"key:"<<key<<endl;
		send_str(key, socket_with_tracker);

		string val; cin>>val; // cout<<"val:"<<val<<endl;
		send_str(val, socket_with_tracker);
	}
}