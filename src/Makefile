OBJS = uploader.o downloader.o protocolhandler.o common.o network.o main.o application.o
quad: $(OBJS)
	g++ -o quad $(OBJS) -lcurses
main.o: main.cpp application.h
	g++ -c main.cpp
uploader.o: uploader.cpp uploader.h
	g++ -c uploader.cpp
protocolhandler.o: protocolhandler.cpp protocolhandler.h
	g++ -c protocolhandler.cpp
downloader.o: downloader.cpp downloader.h
	g++ -c downloader.cpp
network.o: network.cpp network.h
	g++ -c network.cpp
application.o: application.cpp application.h
	g++ -c application.cpp
common.o: common.cpp
	g++ -c common.cpp


.PHONY : clean

clean:
	rm $(OBJS)
.PHONY : instal
install:
	cp quad /usr/bin
