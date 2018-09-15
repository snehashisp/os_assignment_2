#include"tfile.h"

using namespace std;

int main(int argc,char *argv[]) {
	
	string testip = "10.1.10.1";
	string testport = "10232";

	mtorrent newtor;

	newtor.set_tracker(0,testip,testport);
	newtor.set_tracker(1,testip,testport);
	
	newtor.create_from_file(string(argv[1]));
	newtor.create_file(string(argv[2]));

	printf("Hash size : %d",newtor.hash.size());
}

	

