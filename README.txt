1. Xingmin,Han xhan5@binghamton.edu
2. Language:c++
3. Platform: Bingsuns
4. Use makefile to build the program, and first use ./psystem to open the puchasing system, then./bank, and ./customer.
5.The code of encryption/decryption:

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

6.The ip address in the all.hpp named const char* ip_add.
7.This program use map to save the file.
8.All the extra credit work have been done.

