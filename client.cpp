#include"client.h"
using namespace std;
void *client_thread_runner(void *pointer) {

	client_server *temp = (client_server *)pointer;
	temp->client_thread();
	return NULL;
}


client_server :: client_server() {

	pthread_mutex_init(&db_lock,NULL);

}	

void open_file :: open_file_m(mtorrent filet) {

	m_file = filet;
	fp = fopen(filet.file_path.c_str(),"r");
	pthread_mutex_init(&file_lock,NULL);
	peers = 1;
}

mem_file *open_file :: get_data(std::string sub_hash) {

	pthread_mutex_lock(&file_lock);
	auto block = file_db.find(sub_hash);

	if(m_file.file_type == FILE_TYPE_NET) {
		if(block != file_db.end()) {
			(*block).second->mem_access++;
			return (*block).second;
		}
		else return NULL;
	}
		
	if(block == file_db.end()) {

		int file_index = m_file.hash.find(sub_hash.c_str());
		file_index = file_index / 20;
	
		mem_file *new_mem = new mem_file;
		new_mem->block_hash = sub_hash;
		fseek(fp,file_index * SHA_FILE_DIV * BLOCK_SIZE,SEEK_SET);
		new_mem->mem_data = new char[SHA_FILE_DIV * BLOCK_SIZE];
		new_mem->block_size = fread(new_mem->mem_data,1,SHA_FILE_DIV * BLOCK_SIZE,fp);
		new_mem->mem_access++;
		file_db.insert(pair<string,mem_file *>(sub_hash,new_mem));
		pthread_mutex_unlock(&file_lock);
		return new_mem;
	}
	else {
		(*block).second->mem_access++;
		return (*block).second;
	}
	pthread_mutex_unlock(&file_lock);
}

void open_file :: free_file() {

	pthread_mutex_lock(&file_lock);	
	for(auto i = file_db.begin(); i != file_db.end(); i++ ) {
		free((*i).second);
	}
	file_db.clear();
	fclose(fp);
	pthread_mutex_unlock(&file_lock);

}

void client_server :: setup_server(char *tor_file_path,char *ip,int port,char *log_file_path) {

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
		printf("%s\n",strerror(errno));
		return;
	}
	else printf("Bound\n");

	tor_list_file = fopen(tor_file_path,"a+");
	fseek(tor_list_file,0,SEEK_SET);

	char f_path[1024];
	while(fscanf(tor_list_file,"%s",f_path) != EOF) {
		
		mtorrent new_tor;
		new_tor.read_file(string(f_path));
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
			close(new_info.client_socket);
			return;
		}
		pthread_mutex_lock(&db_lock);
		if(pthread_attr_init(&(new_info.thread_attribute)) != 0) {
			printf("Erroe initializing a thread attribute for this client\n");
			close(new_info.client_socket);
			continue;
		}
		else printf("Thread attribute init\n");
		if((pthread_attr_setdetachstate(&(new_info.thread_attribute),PTHREAD_CREATE_DETACHED)) != 0) {
			printf("ERROR etting thread attribute\n");
			close(new_info.client_socket);
			continue;
		}
		else printf("Thread Attribute Set\n");
		client_db.insert(pair<int,client_info>(new_info.client_socket,new_info));
		client_thread_pointer = &(client_db[new_info.client_socket]);
		if(pthread_create(&(new_info.client_thread),&(new_info.thread_attribute),client_thread_runner,(void *)this) != 0) {
			printf("Error in thread creation for the client\n");
			client_db.erase(client_db.find(new_info.client_socket));
			close(new_info.client_socket);
			continue;
		}
		else printf("Client thread created\n");
//		pthread_mutex_unlock(&db_lock);
	}
}

	
void client_server :: client_thread() {

	client_info *client = client_thread_pointer;
	int status = 1;
	char cmd[CMD_LEN];
	char hash[HASH_LEN];

	printf("Thread started for Client socket descriptor %d\n",client->client_socket);
	while(status) {

		write(client->client_socket,CONT,strlen(CONT));
		memset(cmd,0,CMD_LEN);
		memset(hash,0,HASH_LEN);
		read(client->client_socket,cmd,CMD_LEN);
		

		if(strcpy(cmd,"get") == 0) {
			
			write(client->client_socket,CONT,strlen(CONT));
			read(client->client_socket,hash,HASH_LEN);
			
			pthread_mutex_lock(&db_lock);
			auto file = open_file_db.find(string(hash));
			if(file == open_file_db.end()) {
				mtorrent mt_file = torrent_file_db[string(hash)];
				open_file new_open_file;
				new_open_file.open_file_m(mt_file);
				open_file_db.insert(pair<string,open_file>(string(hash),new_open_file));
				file = open_file_db.find(string(hash));
			}
			
			(*file).second.peers++;
			memset(hash,0,HASH_LEN);
			write(client->client_socket,CONT,strlen(CONT));
			read(client->client_socket,hash,HASH_LEN);
			mem_file *ret_file = (*file).second.get_data(string(hash));
			
			if(ret_file) {
				string file_size = to_string(ret_file->block_size);
				write(client->client_socket,file_size.c_str(),file_size.size());
				wait_cont(client->client_socket);
				write(client->client_socket,ret_file->mem_data,ret_file->block_size);
			}
			else {
				char c = '0';
				write(client->client_socket,&c,1);
			}
			pthread_mutex_unlock(&db_lock);

		}
		else if(strcpy(cmd,"ext") == 0) {
			status = 0;
		}
	}
	close(client->client_socket);
	pthread_mutex_lock(&db_lock);
	client_db.erase(client_db.find(client->client_socket));
	pthread_mutex_unlock(&db_lock);	
}

void client_server :: setup_client(char *t1ip,char *t2ip,int t1p,int t2p) {

	tip1 = string(t1ip);
	tip2 = string(t2ip);
	p1 = to_string(t1p);
	p2 = to_string(t2p);

	t1_socket = socket(AF_INET,SOCK_STREAM,0);
	if(t1_socket == -1) {
		printf("Error in tracker 1 socket creation\n");
		return;
	}
	else printf("Tracker 1 socket created\n");
	t2_socket = socket(AF_INET,SOCK_STREAM,0);
	if(t2_socket == -1) {
		printf("Error in tracker 2 socket creation\n");
		return;
	}
	else printf("Tracker 2 socket created\n");

	struct sockaddr_in adrs;
	adrs.sin_family = AF_INET;
	adrs.sin_addr.s_addr = inet_addr(t1ip);
	adrs.sin_port = t1p;
	socklen_t len = sizeof(adrs);
	if(connect(t1_socket,(struct sockaddr*)&adrs,len) == -1) {
		printf("Error connecting tracker 1");
		return;
	}
	else printf("Tracker 1 connected\n");

	/*
	adrs.sin_addr.s_addr = inet_addr(t2ip);
	adrs.sin_port = t2p;
	len = sizeof(adrs);
	if(connect(t2_socket,(struct sockaddr*)&adrs,len) == -1) {
		printf("Error connecting tracker 2");
		return;
	}
	else printf("Tracker 2 connected\n");
	*/
	
}


void client_server :: share_mt_file(mtorrent mfile) {

	wait_cont(t1_socket);
	string cmd = "share";
	write(t1_socket,cmd.c_str(),cmd.size());
	wait_cont(t1_socket);
	write(t1_socket,mfile.file_name.c_str(),mfile.file_name.size());
	wait_cont(t1_socket);
	write(t1_socket,mfile.file_hash.c_str(),mfile.file_hash.size());

}

open_file *client_server :: get(string path) {
	
	mtorrent new_tor;
	if(!new_tor.read_file(path)) {

		printf("mtorrent file not found\n");
		return NULL;
	}
	
	auto test = open_file_db.find(new_tor.file_hash);

	if(test == open_file_db.end()) {
	
	
		wait_cont(t1_socket);
		string cmd="seederlist";
		write(t1_socket,cmd.c_str(),cmd.size());
		wait_cont(t1_socket);
		write(t1_socket,new_tor.file_hash.c_str(),new_tor.file_hash.size());
		wait_cont(t1_socket);
		
		char seed_size[MAX_SEEDL];
		read(t1_socket,seed_size,MAX_SEEDL);
		if(atoi(seed_size) == 0) printf("Torrent %s not available in server\n",new_tor.file_name.c_str());
		else {
			write(t1_socket,CONT,strlen(CONT));
			read(t1_socket,seed_size,MAX_SEEDL);
			open_file new_open_file;
			new_open_file.open_file_m(new_tor);
			char *str = strtok(seed_size," ");
			while(str != NULL && strlen(str) > 1) {
				printf("IP : %s ",str);
				new_open_file.ip_list.push_back(string(str));
				str = strtok(seed_size," ");
				printf("PORT : %s \n ",str);
				new_open_file.port_list.push_back(string(str));
				str = strtok(seed_size," ");
			}
			pthread_mutex_lock(&db_lock);
			open_file_db.insert(pair<string,open_file>(new_tor.file_hash,new_open_file));
			torrent_file_db.erase(torrent_file_db.find(new_tor.file_hash));
			torrent_file_db.insert(pair<string,mtorrent>(new_tor.file_hash,new_tor));
			printf("Added Torrent %s info from server\n",new_tor.file_name.c_str());
			pthread_mutex_unlock(&db_lock);
			return &open_file_db[new_tor.file_hash];
		}
	}
	else return &(test->second); 

}
		



void client_server :: share(string path) {

	mtorrent new_torrent;
	new_torrent.set_tracker(0,tip1,p1);
	new_torrent.set_tracker(0,tip2,p2);
	
	new_torrent.create_from_file(path);
	string mpath = new_torrent.file_path + ".mtorrent";
	new_torrent.create_file(mpath);
	fprintf(tor_list_file,"%s",mpath.c_str());
	
	pthread_mutex_lock(&db_lock);
	torrent_file_db.insert(pair<string,mtorrent>(new_torrent.file_hash,new_torrent));
	pthread_mutex_unlock(&db_lock);
	
	new_torrent.print_data_term();
	share_mt_file(new_torrent);
}


void client_server :: exit() {

	string temp = "exit";
	write(t1_socket,temp.c_str(),temp.size());
	//fclose(tor_list_file);
}



		

	

	
