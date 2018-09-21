#include"tfile.h"

using namespace std;

void mtorrent :: set_tracker(int t_no,string ip,string port) {

	tracker_ip[t_no]   = ip;
	tracker_port[t_no] = port;

}

void mtorrent :: create_from_file(string file_path) {

	char data[SHA_FILE_DIV * BLOCK_SIZE];
	
	char real_path[1024];
	realpath(file_path.c_str(),real_path);
	string temp = string(real_path);
	this->file_name = temp.substr(temp.find_last_of("/")+1);
	this->file_path = temp;

	FILE *file = fopen(file_path.c_str(),"r");
	unsigned char temphash[20];
	file_size = 0;

	hash = "";
	long long temp_size = 0;
	char word[3];
	while((temp_size = fread(data,1,SHA_FILE_DIV * BLOCK_SIZE,file)) > 0) {
		
		SHA1((unsigned char*)data,temp_size,(unsigned char*)temphash);
		for(int i=0;i<20;i++) {
			sprintf(word,"%02x",temphash[i]);	
			hash += string(word);
		}
		file_size += temp_size;

	}

	SHA1((unsigned char*)hash.c_str(),hash.size(),(unsigned char *)temphash);
	file_hash = "";
	for(int i=0;i<20;i++) {
		sprintf(word,"%02x",temphash[i]);
		file_hash += string(word);
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
	fprintf(file,"\n%s",file_hash.c_str());
	fprintf(file,"\n%s",hash.c_str());

	fclose(file);
}


void mtorrent :: read_file(std::string path) {

	FILE *fp = fopen(path.c_str(),"r");
	char buffer[SHA_FILE_DIV * BLOCK_SIZE];
	
	for(int i=0;i<TRACKERS;i++) {
		fscanf(fp,"%s",buffer);
		tracker_ip[i] = string(buffer);
		fscanf(fp,"%s",buffer);
		tracker_port[i] = string(buffer);
	}

	fscanf(fp,"%s",buffer);
	this->file_name = string(buffer);

	fscanf(fp,"%s",buffer);
	file_type = FILE_TYPE_NET;
	
	fscanf(fp,"%s",buffer);
	file_path = string(buffer);
	fscanf(fp,"%s",buffer);
	file_size = atoi(buffer);

	fscanf(fp,"%s",buffer);
	file_hash = string(buffer);
	fscanf(fp,"%s",buffer);
	hash = string(buffer);

}

void mtorrent :: print_data_term() {

	for(int i=0;i<TRACKERS;i++) {

		cout<<tracker_ip[i]<<"\n";
		cout<<tracker_port[i]<<"\n";
	}

	cout<<file_name<<"\n";
	cout<<file_size<<"\n";
	cout<<file_type<<"\n";
	cout<<file_path<<"\n";
	cout<<file_hash<<"\n";
	cout<<hash<<"\n";

}




		
		
			
			


