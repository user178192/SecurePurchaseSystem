/*
 psystem.cpp
 */
#include "all.hpp"

int  listenfd, connfd, clilen;
struct sockaddr_in servaddr, cliaddr;

typedef struct itemInfo{
    string name;
    string value;
    itemInfo(string name,string value){
        this->name = name;
        this->value = value;
    }
}shop_item;       // item struct

map<string,shop_item> item_map;

void show_itemlist();
void handle_selecitem(char* item_number);
void send_card(char* card_number,string price,string cus_name);
char* socket_bank();

int main(int argc, char **argv)
{
	ifstream fin("item.txt");
	assert(fin!=0);
    int pid;
    
    string item_number;
	string item_name;
	string item_value;    //will change to int in bank
	while(fin>>item_number>>item_name>>item_value)
	{
		shop_item one_item(item_name,item_value);
        item_map.insert(pair<string,shop_item>(item_number,one_item));
	}
    
    fin.close();
    
    /* Create a TCP socket */
    char recvline[_MAX_BUFF];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    /* Initialize server's address and well-known port */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(10000);   /* daytime server */
    
    /* Bind server’s address and port to the socket */
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    
    /* Convert socket to a listening socket – max 100 pending clients*/
    listen(listenfd, 100);
    cout<<"psystem hello"<<endl;
    
    /* Wait for client connections and accept them */
    
    while(true)
    {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, (socklen_t*) &clilen);
        if (connfd<0)
        {
            perror("ERROR on accept");
            exit(5);
        }

        if( (pid = fork() ) == 0)   
        {
            close(listenfd);  //Child server;
            show_itemlist(); //display goods to customer
            bzero(recvline, sizeof(recvline));
            if(read(connfd,recvline,sizeof(recvline))>0)
            {
                handle_selecitem(recvline);
            }
            close(connfd);
            exit(0);
        }
            close(connfd);   //Parent server;
    }
    return 0;
}


void send_card(char* card_number,string price,string usr_name)  //send card_number and price to bank
{
    char *ptr_de,*ptr_en,*ptr_price,*c_price;
    char recvline[_MAX_BUFF];
    int  sockfd;
    struct sockaddr_in servaddr;
    
    while(true)
    {
        /* Create a TCP socket */
        if((sockfd=socket(AF_INET,SOCK_STREAM, 0)) < 0)
        {
            perror("socket"); exit(2);
        }
        
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(1200); /* daytime server port */
        
        if (inet_pton(AF_INET,ip_add,&servaddr.sin_addr) <= 0)
        {
            perror("inet_pton"); exit(3);
        }
        
        /* Connect to the server */
        if (connect(sockfd, (struct sockaddr *) &servaddr,sizeof(servaddr))< 0 )
        {
            perror("connect"); exit(4);
        }
        
        ptr_de=public_decrypt(card_number,CUST_PUB_K);
        
        if(ptr_de!=NULL)
        {
            cout<<"Verfiy the customer digital signature"<<endl;
        }
        
        ptr_en=private_encrypt(ptr_de,PSYS_PRI_K);
        
        write(sockfd,ptr_en,_KEY_SIZE);   //send card number
        
        if (ptr_en!=NULL)
        {
            free((char*)ptr_en);
        }
        if (ptr_de!=NULL)
        {
            free((char*)ptr_de);
        }
        
        
        c_price=const_cast<char*>(price.c_str());
        ptr_price=private_encrypt(c_price,PSYS_PRI_K);
        write(sockfd,ptr_price,_KEY_SIZE); //send card price
        
        if (ptr_price!=NULL)
        {
            free((char*)ptr_price);
        }
        
        if(read(sockfd,recvline,sizeof(recvline))>0)
        {
            cout<<recvline<<endl;
        }
        bzero(recvline,sizeof(recvline));
        
        char* ptr_name=const_cast<char*>(usr_name.c_str());
        write(sockfd,ptr_name,(strlen(ptr_name)+1)); //send user name
        
        close(sockfd);
        return;
    }
}


void show_itemlist()
{
	char sendline[_MAX_BUFF]="";
	string output;
    for (map<string,shop_item>::iterator iter=item_map.begin(); iter != item_map.end( ); ++iter)
    {
	    output=(*iter).first+" "+(*iter).second.name+" "+"$"+(*iter).second.value+"\n";
        strncat(sendline,output.c_str(),output.size());
    }
    write(connfd,sendline,sizeof(sendline));
}

void handle_selecitem(char* item_number)
{
    char recvline[_MAX_BUFF];   // the user name;
    string item(item_number);
    string user_name;
    map<string,shop_item>::iterator iter;
    iter = item_map.find(item);
    if(iter!=item_map.end())
    {
        cout<<"The item is :"<<(*iter).second.name<<"  The price is:"<<(*iter).second.value<<endl;
        write(connfd,"Please enter your name:",sizeof("Please enter your name:"));
        assert(read(connfd,recvline, sizeof(recvline))>0); //read user name
        user_name=recvline;
        write(connfd,"Please enter your credit card number:",sizeof("Please enter your Please enter your credit card number:"));
        assert(read(connfd,recvline, sizeof(recvline))>0);//read user credit card number
        send_card(recvline,(*iter).second.value,user_name);
        bzero(recvline,sizeof(recvline));
    }
     else
     {
        write(connfd,"the item not in the system \n",sizeof("the item not in the system \n"));
     }
}

//--------------------------------------------------------------------------------


char *private_encrypt(char *str,char *path_key)
{
    char *p_en;
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if ((file=fopen(path_key,"r"))==NULL)
    {
        perror("open key file error");
        return NULL;
    }
    if((p_rsa=PEM_read_RSAPrivateKey(file,NULL,NULL,NULL))==NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    rsa_len=RSA_size(p_rsa);
    p_en=(char *)malloc(rsa_len);
    bzero(p_en,rsa_len);
    
    assert(RSA_private_encrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_en,p_rsa,RSA_NO_PADDING)!=-1);
    RSA_free(p_rsa);
    fclose(file);
    return p_en;
}


char *public_decrypt(char *str,char *path_key)
{
    char *p_de;
    RSA *p_rsa;
    FILE *file;
    int rsa_len;
    if ((file=fopen(path_key,"r"))==NULL)
    {
        perror("open key file error");
        return NULL;
    }
    if ((p_rsa=PEM_read_RSA_PUBKEY(file,NULL,NULL,NULL))==NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    rsa_len=RSA_size(p_rsa);
    p_de=(char *)malloc(rsa_len);
    bzero(p_de,rsa_len);
    assert(RSA_public_decrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_de,p_rsa,RSA_NO_PADDING)!=-1);
    RSA_free(p_rsa);
    fclose(file);
    return p_de;
}
