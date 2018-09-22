#include"tfile.h"
#include<cstdlib>
#include<map>
#include<cstdio>
#include<unistd.h>
#include<vector>
#include<cstring>
#include<error.h>
#include<pthread.h>
#include<semaphore.h>
struct mem_file {

	int block_size;
	std::string block_hash;
	char *mem_data;
	int mem_access;

	mem_file() {
		mem_access = 0;
		mem_data = NULL;
	}

};

class open_file {
	
	public :

	mtorrent m_file;
	std::map<std::string,mem_file *> file_db;
	FILE *fp;
	std::vector<std::string> ip_list;
	std::vector<std::string> port_list;

	
	int peers = 0;
	pthread_mutex_t file_lock;
	
	void open_file_m(mtorrent filet);
	mem_file *get_data(std::string sub_hash);
	void free_file();
};


class client_server {

	client_info *client_thread_pointer;
	std::string tip1,tip2;
	std::string p1,p2;
	
	int t1_socket,t2_socket;


	public : 

	void share_mt_file(mtorrent mfile);	
	
	int server_status;	
	int serv_sockid;
	struct sockaddr_in address;
	std::map<int,client_info> client_db;
	std::map<std::string,open_file> open_file_db;
	std::map<std::string,mtorrent> torrent_file_db;
	FILE *tor_list_file;

	pthread_mutex_t db_lock;
	client_server();

	void setup_server(char *tor_file_path,char *ip,int port,char *log_file_path);
	void setup_client(char *t1ip,char *t2ip,int t1p,int t2p);
	void client_thread();
	void server_thread();
	
	void share(std::string path);
	open_file *get(std::string path);
	void exit();
	void cmd_thread();
};

