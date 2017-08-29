/*
 ============================================================================
 Name        : NTRU-KEM.c
 Author      : zhenfei
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "param.h"
#include "NTRUEncrypt.h"
#include "api.h"


int get_len(char *c)
{
    int len = 0;
    while(c[len]!='\0')
        len++;
    return len;
}

int test_kem(PARAM_SET *param) {

    uint16_t    *F, *g, *h, *buf, *m, *c, *mem;
    uint16_t    i;

    printf("===============================\n");
    printf("===============================\n");
    printf("===============================\n");

    mem     = malloc (sizeof(uint16_t)*param->padN * 5);
    buf     = malloc (sizeof(uint16_t)*param->padN * 6);
    if (!mem || !buf )
    {
        printf("malloc error!\n");
        return -1;
    }

    memset(buf, 0, sizeof(uint16_t)*param->padN * 5);

    F       = mem;
    g       = F     + param->padN;
    h       = g     + param->padN;
    m       = h     + param->padN;
    c       = m     + param->padN;

    printf("start key gen\n");


    keygen(F,g,h,buf,param);

    printf("f:\n");
    for (i=0;i<param->padN;i++)
    {
        printf("%2lld, ",(long long) F[i]);
    }
    printf("\ng:\n");

    for (i=0;i<param->padN;i++)
    {
        printf("%2lld, ",(long long) g[i]);
    }
    printf("\nh:\n");

    for (i=0;i<param->padN;i++)
    {
        printf("%2lld, ",(long long) h[i]);
    }
    printf("\nfinished key gen\n");

    check_keys(F,g,h,buf,param);
    printf("\n");

    trinary_poly_gen(m, param->N, param->d);


    printf("\nmsg:\n");

    for (i=0;i<param->N;i++)
    {
        printf("%2lld, ",(long long) m[i]);
    }
    printf("\n");


    memset(buf, 0, sizeof(uint16_t)*param->padN * 5);
    encrypt_kem(m, h, c, buf, param);


    printf("\nc:\n");

    for (i=0;i<param->N;i++)
    {
        printf("%2lld, ",(long long) c[i]);
    }
    printf("\nfinished encrypt\n");


    memset(buf, 0, sizeof(uint16_t)*param->padN * 5);
    decrypt_kem(m, F, c, buf, param);



    printf("\nrecovered message:\n");

    for (i=0;i<param->N;i++)
    {
        printf("%2d, ",m[i]);
    }
    printf("\nfinished decrypt\n");


    free(mem);
    free(buf);
    return EXIT_SUCCESS;
}

int test_cca (PARAM_SET *param)
{

    uint16_t    *F, *g, *h, *buf, *c, *mem;
    char        *msg, *msg_rec;
    size_t      msg_len;

    uint16_t    i;

    printf("===============================\n");
    printf("===============================\n");
    printf("===============================\n");
    msg     = "nist submission";
    msg_len = get_len(msg);
    msg_rec = malloc (sizeof(char)*param->max_msg_len);
    mem     = malloc (sizeof(uint16_t)*param->padN * 4);
    buf     = malloc (sizeof(uint16_t)*param->padN * 8);
    if (!mem || !buf || !msg_rec)
    {
        printf("malloc error!\n");
        return -1;
    }

    memset(buf, 0, sizeof(uint16_t)*param->padN * 7);
    F       = mem;
    g       = F     + param->padN;
    h       = g     + param->padN;
    c       = h     + param->padN;

    keygen(F,g,h,buf,param);

    printf("f:\n");
    for (i=0;i<param->padN;i++)
    {
        printf("%2lld, ",(long long) F[i]);
    }
    printf("\ng:\n");

    for (i=0;i<param->padN;i++)
    {
        printf("%2lld, ",(long long) g[i]);
    }
    printf("\nh:\n");

    for (i=0;i<param->padN;i++)
    {
        printf("%2lld, ",(long long) h[i]);
    }
    printf("\nfinished key gen\n");


    memset(buf, 0, sizeof(uint16_t)*param->padN * 7);
    check_keys(F,g,h,buf,param);


    memset(buf, 0, sizeof(uint16_t)*param->padN * 7);
    encrypt_cca(c, msg, msg_len, h,  buf, param);


    msg_len = 0;
    memset(buf, 0, sizeof(uint16_t)*param->padN * 7);
    msg_len = decrypt_cca(msg_rec,  F, h,c,  buf, param);
    printf("string of %d chars: %s\n", (int)msg_len, msg_rec);


    free(mem);
    free(buf);

    puts("!!!Hello OnBoard Security!!!"); /* prints !!!Hello OnBoard Security!!! */
    return EXIT_SUCCESS;
}

int test_nist()
{

    printf("===============================\n");
    printf("===============================\n");
    printf("===============================\n");
    printf("testing NIST API\n");

    int     i;
    unsigned char       *msg, *m, *c;
    unsigned long long  msg_len, c_len;
    msg     = (unsigned char*)"nist submission";
    msg_len = get_len((char*)msg);


    c_len   = 1;

    unsigned char       *pk, *sk;

    pk  = malloc(sizeof(unsigned char)* 4000);
    sk  = malloc(sizeof(unsigned char)* 4000);
    m   = malloc(sizeof(unsigned char)* 4000);
    c   = malloc(sizeof(unsigned char)* 4000);

    printf("Let's try to encrypt a message: %s\n", msg);

    crypto_encrypt_keypair(pk, sk);
    printf("key generated, public key:\n");

    for (i=0;i<1022;i++)
        printf("%d, ", (int)pk[i]);
    printf("\n");

    crypto_encrypt(c, &c_len,  msg, msg_len, pk);
    printf("encryption complete, ciphtertext of length %d:\n", (int) c_len);
    for (i=0;i<c_len;i++)
        printf("%d, ", (int)c[i]);
    printf("\n");


    msg_len = 0;
    crypto_encrypt_open(m, &msg_len, c, c_len, sk);

    printf("recovered message with length %d: ", (int)msg_len );
/*    for (i=0;i<msg_len;i++)
        printf("%c", m[i]);
    printf("\n");*/
    puts("!!!Hello OnBoard Security!!!");
    return 0;
}


int main(void)
{

    PARAM_SET *param;
    param   = get_param_set_by_id(NTRU_KEM_443);
    test_kem(param);

    param   = get_param_set_by_id(NTRU_KEM_743);
    test_kem(param);

    param   = get_param_set_by_id(NTRU_CCA_443);
    test_cca(param);

    param   = get_param_set_by_id(NTRU_CCA_743);
    test_cca(param);

    test_nist();

}

