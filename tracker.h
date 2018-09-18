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

#define CONT "continue"
#define CMD_LEN    256
#define IPLEN       20
#define POLEN       10
#define FILE_NAME  256
#define HASH_LEN  1024
#define SERV_LISTEN  1
#define SERV_CLOSED  2
#define SERV_ACCEPT  3
#define SERV_UNINIT  4
#define SERV_INIT    5

struct client_info {

	int client_socket;
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


	void send_file_data(client_info info);
	void create_new_tor(client_info info);
	
	void client_thread(client_info *client);	

	public:

	int serv_sockid;
	std::map<int,client_info> client_db;
	std::map<std::string,mtorrent_info> torrent_db;
	int total_clients;
	struct sockaddr_in address;
	int server_status;

	std::string tracker_name;
	
	FILE *torrent_list_file;
	FILE *log_file;

	void setup(char *ip,int port,char *torrent_list_file_path,char *log_file_path);
	void runserv();
	void close_connection();
};





