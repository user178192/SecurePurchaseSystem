CC=g++
CFLAGS=-Wall -g 
INCLUDE=-I/usr/local/ssl/include/
LIB=-L/usr/local/ssl/lib -lcrypto -lssl -lsocket -lnsl 
 
all: customer psystem bank
 
customer: customer.cpp all.hpp
	$(CC) $(CFLAGS) customer.cpp -o $@ $(LIB) $(INCLUDE)

psystem: psystem.cpp all.hpp
	$(CC) $(CFLAGS) psystem.cpp -o $@ $(LIB) $(INCLUDE)

bank: bank.cpp all.hpp
	$(CC) $(CFLAGS) bank.cpp -o $@ $(LIB) $(INCLUDE)
 
clean:
	@rm -f customer psystem bank
