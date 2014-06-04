#include "all.hpp"


map<string,long> account_map;
int  listenfd, connfd, clilen;
struct sockaddr_in servaddr, cliaddr;
string renew_account(char* card_number,char* price);
void  renew_activity(string name,string price);

string str_success="Your credit card number is verified and we will process your order soon";
string str_fail="Wrong credit card number";
string str_wrong="Not enough money";

int main()
{
    char sendline[_MAX_BUFF]="";
    string return_value;
    int pid;
    string user_name,item_price;
    
    /* Create a TCP socket */
    char *ptr_price,*ptr_dprice,*ptr_de1,*ptr_en,*ptr_de2;
    char recvline[_MAX_BUFF];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    /* Initialize server's address and well-known port */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(1200);   /* daytime server */
    
    /* Bind server’s address and port to the socket */
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    
    /* Convert socket to a listening socket – max 100 pending clients*/
    listen(listenfd, 100);
    cout<<"bank hello"<<endl;
    
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
            
            bzero(recvline, sizeof(recvline));  //read card number from psystem
            
            if(read(connfd,recvline,sizeof(recvline))>0)
            {
                ptr_en=recvline;
                ptr_de1=public_decrypt(ptr_en,PSYS_PUB_K);
                if (ptr_de1!=NULL) {
                    cout<<"Verfiy the psystem digital signature"<<endl;
                }
                ptr_de2=private_decrypt(ptr_de1,BANK_PRI_K);
                bzero(recvline,sizeof(recvline));
            }
            if(read(connfd,recvline,sizeof(recvline))>0)
            {
                
                ptr_price=recvline;
                ptr_dprice=public_decrypt(ptr_price,PSYS_PUB_K);
                item_price=ptr_dprice;
                return_value=renew_account(ptr_de2,ptr_dprice);
                strncat(sendline,return_value.c_str(),return_value.size());
                write(connfd,sendline,strlen(sendline));
                bzero(recvline,sizeof(recvline));
            }
            
            if(read(connfd,recvline,sizeof(recvline))>0)
            { 
                cout<<"Customer:"<<recvline<<endl;
                user_name=recvline;
               if(return_value==str_success)  
                {
                    renew_activity(user_name,item_price);
                }
                bzero(recvline,sizeof(recvline));
            }
            close(connfd);
            exit(0);
        }
        close(connfd);//Parent server;
    }
    return 0;
}


void  renew_activity(string name,string price)
{
    ofstream fout("activity.txt",std::ios::out|std::ios::app);
    time_t   now;          
    tm  *timenow;          
    time(&now);
    timenow=localtime(&now);
    fout<<name<<" "<<timenow->tm_mon<<"/"<<timenow->tm_mday<<"/"<<timenow->tm_year+1900<<"  $"<<price<<endl;
}

string  renew_account(char* card_number,char* price)
{
    ifstream fin("balance.txt");
    if (fin==0) {
        cout<<"Error opening input file exiting\n"<<endl;
        exit(1);
    }
    string credit_num,return_value;
	long item_value;
	while(fin>>credit_num>>item_value)
	{
        account_map.insert(pair<string,long>(credit_num,item_value));
	}
    fin.close();
    
    
    map<string,long>::iterator iter;
    string one_account(card_number);
    free((char*)card_number);
    int item_price=atoi(price);
    cout<<"The card number is:"<<one_account<<endl;
    cout<<"The item price is:"<<item_price<<"$"<<endl;
    iter = account_map.find(one_account);
    if(iter!=account_map.end())
    {
        cout<<"The account balance is:"<<(*iter).second<<"$"<<endl;
        if ((*iter).second-item_price>0)
        {
            (*iter).second=(*iter).second-item_price;
            cout<<"The account balance is:"<<(*iter).second<<"$"<<endl;
            ofstream fout("balance.txt");
            if (fout==0) {
                cout<<"Error opening input file exiting\n"<<endl;
                exit(1);
            }
            
            for (iter=account_map.begin( ); iter!= account_map.end( ); ++iter)
            {
                fout<< (*iter).first << "   " <<(*iter).second << endl;
            }
            fout.close();
            return str_success;
        }
        else
            return str_wrong;
    }
    else
    {
        return str_fail;
    }
}


//--------------------------------------------------------------------------------


char *private_decrypt(char *str,char *path_key)
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
    if ((p_rsa=PEM_read_RSAPrivateKey(file,NULL,NULL,NULL))==NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    rsa_len=RSA_size(p_rsa);
    p_de=(char *)malloc(rsa_len);
    bzero(p_de,rsa_len);
    assert(RSA_private_decrypt(rsa_len,(unsigned char *)str,(unsigned char*)p_de,p_rsa,RSA_NO_PADDING)!=-1);
    RSA_free(p_rsa);
    fclose(file);
    return p_de;
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

