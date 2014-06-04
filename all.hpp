#ifndef _ALL_H_
#define _ALL_H_
#define _KEY_SIZE 128
#define _MAX_BUFF 2048
#define PSYS_PUB_K "Pup.key"
#define PSYS_PRI_K "Prp.key"
#define BANK_PUB_K "Pub.key"
#define BANK_PRI_K "Prb.key"
#define CUST_PUB_K "Puc.key"
#define CUST_PRI_K "Prc.key"

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <fstream>
#include <iterator>
#include <assert.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

using namespace std;

const char* ip_add="128.226.6.39";
char *public_encrypt(char *str,char *path_key);
char *private_encrypt(char *str,char *path_key);
char *public_decrypt(char *str,char *path_key);
char *private_decrypt(char *str,char *path_key);

#endif
