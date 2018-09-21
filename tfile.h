#include<cstdio>
#include<string>
#include<openssl/sha.h>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define FILE_TYPE_DISK 1
#define FILE_TYPE_NET  2
#define SHA_FILE_DIV   512
#define TRACKERS       2
#define BLOCK_SIZE     1024

struct client_info {

        int client_socket;
        pthread_t client_thread;
        pthread_attr_t thread_attribute;
        struct sockaddr_in address;
        
};


class mtorrent {
	
	public :

	std::string tracker_ip[TRACKERS];
	std::string tracker_port[TRACKERS];

	std::string file_name;
	long long   file_size;
	std::string file_hash;
	std::string hash;
	int         file_type;
	std::string file_path;

	void set_tracker(int t_no,std::string ip,std::string port);	
	void create_from_file(std::string file_path);
	void create_file(std::string path);
		
	void read_file(std::string path);	
	void print_data_term();	
};


