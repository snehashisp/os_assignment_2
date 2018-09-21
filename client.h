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


#define CONT   "continue"
#define CMD_LEN      256
#define IPLEN         20
#define POLEN         10
#define FILE_NAME    256
#define HASH_LEN    1024
#define M_HASH_LEN 65536

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
	
	mtorrent m_file;
	std::map<std::string,mem_file *> file_db;
	FILE *fp;

	public:
	
	pthread_mutex_t file_lock;

	open_file(mtorrent filet);
	mem_file *get_data(std::string sub_hash);
	void free_file();
}


class client_server {

	client_info *client_thread_pointer;

	public : 
	
	int server_status;	
	int serv_sockid;
	struct sockaddr_in address;
	std::map<int,client_info> client_db;
	std::map<std::string,open_file> open_file_db;
	std::map<std::string,mtorrent> torrent_file_db;
	FILE *tor_list_file;
	pthread_mutex_t db_lock;

	void setup(char *tor_file_path,char *ip,int port,char *log_file_path);
	void client_thread();
	void server_thread();
		
}
