a.out : tfile.h tfile.cpp test.cpp tracker.cpp tracker.h servtest.cpp
	g++ -w -c tfile.cpp
	g++ -w -c test.cpp
	g++ -w -c tracker.cpp
	g++ -w -o server servtest.cpp tracker.o tfile.o -lssl -lcrypto
	g++ test.o tfile.o -lssl -lcrypto
