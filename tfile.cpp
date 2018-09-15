#include"tfile.h"

using namespace std;

void mtorrent :: set_tracker(int t_no,string ip,string port) {

	tracker_ip[t_no]   = ip;
	tracker_port[t_no] = port;

}

void mtorrent :: create_from_file(string file_path) {

	char data[SHA_FILE_DIV * BLOCK_SIZE];
	this->file_path = file_path;
	FILE *file = fopen(file_path.c_str(),"r");
	char temphash[20];
	strcpy(temphash,"");
	file_size = 0;

	hash = "";
	long long temp_size = 0;
	while((temp_size = fread(data,1,SHA_FILE_DIV * 1024,file)) > 0) {
		
		SHA1((unsigned char*)data,temp_size,(unsigned char*)temphash);
		hash = hash + string(temphash);
		file_size += temp_size;
	}
	
	file_type = FILE_TYPE_DISK;

}

void mtorrent :: create_file(string path) {

	FILE *file = fopen(path.c_str(),"w");
	for(int i=0;i<TRACKERS;i++) {
		fprintf(file,"\n%s ",tracker_ip[i].c_str());
		fprintf(file,"\n%s",tracker_port[i].c_str());
	}
	fprintf(file,"\n%s",file_name.c_str());
	
	if(file_type == FILE_TYPE_NET) fprintf(file,"\n%s","NET");
	else if(file_type == FILE_TYPE_DISK) fprintf(file,"\n%s","DISK");

	fprintf(file,"\n%s",file_path.c_str());
	fprintf(file,"\n%lli",file_size);
	fprintf(file,"\n%s",hash.c_str());

	fclose(file);
}



		
		
			
			


