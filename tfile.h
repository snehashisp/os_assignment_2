#include<cstdio>
#include<string>
#include<openssl/sha.h>
#include<cstring>
#include<cstdlib>
#include<iostream>

#define FILE_TYPE_DISK 1
#define FILE_TYPE_NET  2
#define SHA_FILE_DIV   512
#define TRACKERS       2
#define BLOCK_SIZE     1024

class mtorrent {
	
	public :

	std::string tracker_ip[TRACKERS];
	std::string tracker_port[TRACKERS];

	std::string file_name;
	long long   file_size;
	std::string hash;
	int         file_type;
	std::string file_path;

	void set_tracker(int t_no,std::string ip,std::string port);	
	void create_from_file(std::string file_path);
	void create_file(std::string path);
	
	void read_file(std::string path);	
	void print_data_term();	
};


