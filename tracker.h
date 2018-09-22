#include"tfile.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstdlib>
#include<map>
#include<cstdio>
#include<unistd.h>
#include<vector>
#include<cstring>
#include<error.h>
#include<pthread.h>
#include<semaphore.h>



struct mtorrent_info {

	std::string file_name;
	std::string torrent_hash_file;
	FILE *client_list;

	std::vector<std::string> ip_list;
	std::vector<std::string> port_list;
	int total_seeds;

	void get_client_list();
	void write_client_list();
};


class tracker {


	
	public:

	client_info *client_thread_info;

	void send_file_data(client_info info);
	void create_new_tor(client_info info);
	void client_thread();	
	pthread_mutex_t db_lock; 
	

	int server_status;
	int serv_sockid;
	std::map<int,client_info> client_db;
	std::map<std::string,mtorrent_info> torrent_db;
	struct sockaddr_in address;

	std::string tracker_name;
	
	FILE *torrent_list_file;
	FILE *log_file;

	void setup(char *ip,int port,char *torrent_list_file_path,char *log_file_path);
	void runserv();
	void close_connection();

};

void *thread_runner(void *trac);



