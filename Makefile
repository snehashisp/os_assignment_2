a.out : tfile.h tfile.cpp test.cpp 
	g++ -c tfile.cpp
	g++ -c test.cpp
	g++ test.o tfile.o -lssl -lcrypto
