all: server client
server:TcpServer.cpp
	g++ $^ -o $@ 
client:TcpClient.cpp
	g++ $^ -o $@

.PHONY:clean
clean:
	-rm server client