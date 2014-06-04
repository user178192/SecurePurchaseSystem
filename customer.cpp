/*
 customer.cpp
 */
#include "all.hpp"

int  sockfd;
struct sockaddr_in servaddr;
char usr_choic[_MAX_BUFF];   //choice item number


void read_itemlist();
void select_item();
char* encrypted_card(string card_number);

void read_itemlist()
{
    char recvline[_MAX_BUFF];
    int flag=read(sockfd,recvline, sizeof(recvline));
    cout<<recvline<<endl;
    if (flag<0)
    {
        perror("ERROR on read socket");
        return;
    }

    bzero(recvline, sizeof(recvline));
}

void select_item()
{
    char recvline[_MAX_BUFF];
    char sendline[_MAX_BUFF]="";
    string item_number,user_name,card_number;
    cout<<"please enter the item number:";

    cin>>item_number;
    write(sockfd,item_number.c_str(),item_number.size());
    assert(read(sockfd,recvline, sizeof(recvline))>0);

    cout<<recvline<<endl;
    if (strncmp(recvline,"the item not",12)==0)
    {
        bzero(recvline, sizeof(recvline));
        return;
    }
    bzero(recvline, sizeof(recvline));

    cin>>user_name;
    strncat(sendline,user_name.c_str(),user_name.size());
    write(sockfd,sendline,sizeof(sendline));

    assert(read(sockfd,recvline, sizeof(recvline))>0);
    cout<<recvline;
    bzero(recvline, sizeof(recvline));

    cin>>card_number;
    char* encrypted_number=encrypted_card(card_number);
    write(sockfd,encrypted_number,_KEY_SIZE);
    if (encrypted_number!=NULL)
    {
        free((char*)encrypted_number);
    }
}

char * encrypted_card(string card_number)
{
     char *ptr_en1,*ptr_en2;
     char* text=const_cast<char*>(card_number.c_str());
     ptr_en1=public_encrypt(text,BANK_PUB_K);
     ptr_en2=private_encrypt(ptr_en1,CUST_PRI_K);
     return ptr_en2;
}

int main(int argc, char **argv) {

  while(true)
    {
        /* Create a TCP socket */
        if((sockfd=socket(AF_INET,SOCK_STREAM, 0)) < 0)
        {
            perror("socket"); exit(2);
        }
        
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(10000); /* daytime server port */
        
        if (inet_pton(AF_INET,ip_add,&servaddr.sin_addr) <= 0)
        {
            perror("inet_pton"); exit(3);
        }
        
        /* Connect to the server */
        if (connect(sockfd, (struct sockaddr *) &servaddr,sizeof(servaddr))< 0 )
        {
            perror("connect"); exit(4);
        }
        
        read_itemlist();
        select_item();
        close(sockfd);
    }
    
} 

//--------------------------------------------------------------------------------

char *public_encrypt(char *str,char *path_key)
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
    
	if ((p_rsa=PEM_read_RSA_PUBKEY(file,NULL,NULL,NULL))==NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }

    rsa_len=RSA_size(p_rsa);
    p_en=(char *)malloc(rsa_len);
    bzero(p_en,rsa_len);
    
    assert(RSA_public_encrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_en,p_rsa,RSA_NO_PADDING)!=-1);
    RSA_free(p_rsa);
    fclose(file);
    return p_en;
}


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

