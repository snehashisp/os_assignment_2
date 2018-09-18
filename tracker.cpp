#include"tracker.h"

using namespace std;

void mtorrent_info :: get_client_list() {

	client_list = fopen(torrent_hash_file.c_str(),"a+");
	char ip[IPLEN];
	char port[POLEN];
	
	fscanf(client_list,"%s",ip);
	fscanf(client_list,"%s",port);	

	while(!feof(client_list)) {
		
		ip_list.push_back(string(ip));
		port_list.push_back(string(port));
		total_seeds++;
		fscanf(client_list,"%s",ip);
		fscanf(client_list,"%s",port);
	}
	
		
}
	


void tracker :: setup(char *ip,int port,char *torrent_list_file_path,char *log_file_path) {

	if((serv_sockid = socket(AF_INET,SOCK_STREAM,0)) == -1) {

		printf("%s",strerror(errno));
		return ;
	}
	else printf("Socket Created\n");

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(ip);
	address.sin_port = port;
	socklen_t len = sizeof(address);

	if((bind(serv_sockid,(struct sockaddr *)&address,len)) == -1) {
		
		printf("%s",strerror(errno));
		return ;
	}
	else printf("Bound\n");


	
	torrent_list_file = fopen(torrent_list_file_path,"a+");
	log_file = fopen(log_file_path,"a+");

	char line[HASH_LEN];
	
	while(fscanf(torrent_list_file,"%s",line) != EOF) {
		mtorrent_info new_info;
		new_info.file_name = string(line);
		fscanf(torrent_list_file,"%s",line);
		new_info.torrent_hash_file = string(line);
		//new_info.get_client_list();
		torrent_db.insert(pair<string,mtorrent_info>(new_info.torrent_hash_file,new_info));
	}

	
	printf("Created torrent Database\n");
	
}



void tracker :: runserv() {

	if(listen(serv_sockid,5) == -1) {
		printf("%s",strerror(errno));
		return;
	}
	else printf("server Listening\n");
	server_status = SERV_LISTEN;
	

	while(server_status == SERV_LISTEN) {
		
		client_info new_info;
		socklen_t len = sizeof(new_info.address);
		new_info.client_socket = accept(serv_sockid,(struct sockaddr *)&new_info.address,&len);
		if(new_info.client_socket == -1) {
			printf("%s",strerror(errno));
			return;
		}
		else {
			printf("Connection Established\n");
			client_db.insert(pair<int,client_info>(new_info.client_socket,new_info));
			client_thread(&(client_db[new_info.client_socket]));
		}
		
	}

}

void tracker :: client_thread(client_info *client_pointer) {

	client_info *client = (client_info *)client_pointer;
	int status = 1;
	char line[CMD_LEN];
	
	printf("Client ID %d\n",client->client_socket);
	while(status) {
			
		write(client->client_socket,CONT,strlen(CONT));
		memset(line,0,CMD_LEN);
		read(client->client_socket,line,CMD_LEN);
		
		char *cmd1 = line;
		while(*cmd1 && *cmd1 == ' ') cmd1++;
		cmd1 = strtok(cmd1," ");
		if(strcmp(cmd1,"share") == 0) {
			create_new_tor(*client);
		}
		else if(strcmp(cmd1,"seederlist") == 0) {
			send_file_data(*client);
		}
		else if(strcmp(cmd1,"remove") == 0) {
			
			write(client->client_socket,CONT,strlen(CONT));
			char hash[HASH_LEN];
			read(client->client_socket,hash,HASH_LEN);
			auto rm = torrent_db.find(string(hash));
			
			if(rm == torrent_db.end()) continue;
			else {
				fclose(rm->second.client_list);
				unlink(rm->second.torrent_hash_file.c_str());
				torrent_db.erase(rm);
			}
		}
		else if(strcmp(cmd1,"exit") == 0) status = 0;
			
	}

	printf("Client ID %d Disconnected\n",client->client_socket);
	close(client->client_socket);
	client_db.erase(client_db.find(client->client_socket));
	
}


void tracker :: send_file_data(client_info info) {

	string list;

	printf("seederlist\n");
	return;
	write(info.client_socket,CONT,strlen(CONT));
	char hash[HASH_LEN];
	read(info.client_socket,hash,HASH_LEN);

	if(torrent_db.find(string(hash)) == torrent_db.end()) return;

	mtorrent_info minfo = torrent_db[string(hash)];
	for(int i=0;i<minfo.ip_list.size();i++)
		list += minfo.ip_list[i] + "," + minfo.port_list[i] + ";";
	write(info.client_socket,list.c_str(),list.size());
	minfo.total_seeds += 1;

	minfo.ip_list.push_back(string(inet_ntoa(address.sin_addr)));
	minfo.port_list.push_back(to_string(address.sin_port));
	
}


void tracker :: create_new_tor(client_info info) {
	
	printf("share\n");
	char hash[HASH_LEN];
	char filename[FILE_NAME];
	
	write(info.client_socket,CONT,strlen(CONT));
	read(info.client_socket,filename,FILE_NAME);
	write(info.client_socket,CONT,strlen(CONT));
	read(info.client_socket,hash,HASH_LEN);

	mtorrent_info new_info;
	new_info.file_name = string(filename);
	new_info.torrent_hash_file = string(hash);
	new_info.ip_list.push_back(string(inet_ntoa(address.sin_addr)));
	new_info.port_list.push_back(to_string(address.sin_port));
	new_info.total_seeds = 1;

	torrent_db.insert(pair<string,mtorrent_info>(new_info.torrent_hash_file,new_info));
	fprintf(torrent_list_file,"\n%s",filename);
	fprintf(torrent_list_file,"\n%s",hash);

}


void tracker :: close_connection() {
	
	fclose(torrent_list_file);
	fclose(log_file);
	
}


