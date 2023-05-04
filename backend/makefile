ALL:Mysql 

Mysql:mysql.cc
	g++ -o $@ $^ -std=c++11 -lmysqlclient -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -rf Mysql
