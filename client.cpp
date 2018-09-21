#include"tfile.h"
#include"client.h"

void thread_runner(void *pointer) {

	client_server *temp = (client_server *)pointer;
	temp->client_thread();
	return;
}
		
bool wait_cont(int sockid) {

	string temp = "";
	char t;
	do{
		read(sockid,&t,1);
		temp += string(&t);
	}while(strlen(temp) < strlen(CONT));

	if(strcmp(temp.c_str(),CONT) == 0) return true;
	else return false;
}

	

void open_file :: open_file(mtorrent filet) {

	m_file = filet;
	fp = fopen(filet.file_path.c_str(),"r");
	pthread_mutex_init(&file_lock,NULL);
}

mem_file *open_file :: get_data(std::string sub_hash) {

	pthread_mutex_lock(&file_lock);
	auto block = file_db.find(sub_hash);

	if(block == file_db.end()) {
		int file_index = m_file.hash.find(sub_hash.c_str());
		file_index = file_index / 20;
		
		mem_file *new_mem = new nem_file;
		new_mem->block_hash = sub_hash;
		fseek(fp,file_index * SHA_FILE_DIV * BLOCK_SIZE,SEEK_SET);
		new_mem->mem_data = new char[SHA_FILE_DIV * BLOCK_SIZE];
		new_mem->block_size = fread(new_mem->mem_data,1,SHA_FILE_DIV * BLOCK_SIZE,fp);
		mem_access++;
		file_db.insert(pair<string,mem_file>(sub_hash,&new_mem));
		pthread_mutex_unlock(&file_lock);
		return new_mem;
	}
	else {
		(*block)->second->mem_access++;
		return (*block)->second;
	}
	pthread_mutex(&file_lock);
}

void open_file :: free_file() {

	pthread_mutex_lock(&file_lock);	
	for(auto i = file_db.begin(); i != file_db.end(); i++ ) {
		free((*i)->second);
	}
	file_db.clear();
	fclose(fp);
	pthread_mutex_unlock(&file_lock);

}

void client_server :: setup(char *tor_file_path,char *ip,int port,char *log_file_path) {

	if((serv_sockid = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		printf("%s\n",strerror(errno));
		return;
	}
	else printf("Socket Created\n");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = port;
	socklen_t len = sizeof(address);

	if((bind(serv_sockid,(struct sockaddr *)&address,len)) == -1) {
		printf("%s\n",strerror(erron));
		return;
	}
	else printf("Bound\n");

	tor_list_file = fopen(tor_file_path,"r");
	
	char f_path[1024];
	while(fscanf(tor_list_file,"%s",f_path) != EOF) {
		
		mtorrent new_tor;
		new_tor.read_file(string(f_path);
		torrent_file_db.insert(pair<string,mtorrent>(new_tor.file_hash,new_tor));

	}
	pthread_mutex_init(&db_lock,NULL);
	server_status = 1;
}

void client_server :: server_thread() {
	
	if(listen(serv_sockid,5) == -1) {
		printf("%s\n",strerror(errno));
		return;
	}
	else printf("Server Listening\n");

	while(server_status) {
		
		client_info new_info;
		socklen_t len = sizeof(new_info.address);
		new_info.client_socket = accept(serv_sockid,(struct sockaddr*)&new_info.address,&len);
		if(new_info.client_socket == -1) {
			printf("%s",strerror(errno));
			close(mew_info.client_socket);
			return;
		}
		pthread_mutex_lock(&db_lock);
		if(pthread_attr_init(&(new_info.thread_attribute)) != 0) {
			printf("Erroe initializing a thread attribute for this client\n");
			close(new_info.client_socket);
			continue;
		}
		else printf("Thread attribute init\n");
		if((pthread_attr_setdetachstate(&(new)info.thread_attribute),PTHREAD_CREATE_DETACHED)) != 0) {
			printf("ERROR etting thread attribute\n");
			close(new_info.client_socket);
			continue;
		}
		else printf("Thread Attribute Set\n");
		client_db.insert(pair<int,client_info>(new_info.client_socket,new_info));
		client_thread_pointer = &(client_db[new_info.client_socket]);
		if(pthread_create(&(new_info.client_thread),&(new_info.thread_attribute),thread_runner,(void *)this) != 0) {
			printf("Error in thread creation for the client\n");
			client_db.erase(client_db.find(new_info.client_socket));
			close(new_info.client_socket);
			continue;
		}
		else printf("Client thread created\n");
		pthread_mutex_unlock(&db_lock);
	}
}

	
void client_server :: client_thread() {

	client_info *client = client_thread_pointer;
	int status = 1;
	char cmd[CMD_LEN];
	char hash[HASH_LEN];

	printf("Thread started for Client socket descriptor %d\n",client->client->socket);
	while(status) {

		write(client->client_socket,CONT,strlen(CONT));
		memset(cmd,0,CMD_LEN);
		memset(hash,0,HASH_LEN);
		read(client->client_socket,cmd,CMD_LEN);
		
		if(strcpy(cmd,"get") == 0) {
			
			write(client->client_socket,CONT,strlen(CONT));
			read(client->client_socket,hash,HASH_LEN);
			
			auto file = open_file_db.find(string(hash));
			if(file == open_file_db.end()) {
				mtorrent mt_file = torrent_file_db[string(hash)];
				open_file new_open_file(mt_file);
				open_file_db.insert(pair<string,open_file>(string(hash),new_open_file));
				file = open_file_db.find(string(hash));
			}
			
			memset(hash,0,HASH_LEN);
			write(client->client_socket,CONT,strlen(CONT));
			read(client->client_socket,hash,HASH_LEN);
			mem_file *ret_file = (*file).get_data(string(hash));
			
			string file_size = to_string(ret_file->block_size);
			write(client->client_socket,file_size.c_str(),file_size.size());
			wait_cont(client->client_socket);
			write(client->client_socket,ret_file->mem_data,ret_file->block_size);
		}
				






		

	

	
