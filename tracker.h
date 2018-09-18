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


#define CONT "continue"
#define CMD_LEN    256
#define IPLEN       20
#define POLEN       10
#define FILE_NAME  256
#define HASH_LEN  1024

struct client_info {

	int client_socket;
	pthread_t client_thread;
	pthread_attr_t thread_attr;
	struct sockaddr_in address;
	
};

struct mtorrent_info {

	std::string file_name;
	std::string torrent_hash_file;
	FILE *client_list;

	std::vector<std::string> ip_list;
	std::vector<std::string> port_list;
	int total_seeds;

	void get_client_list();
	void delfile();
};


class tracker {

	public:

	void send_file_data(client_info info);
	void create_new_tor(client_info info);
	void client_thread(void *client_pointer);	
	pthread_mutex_t db_lock; 


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





