//g++ -Wall -o client  client_ssl.cpp -L/usr/lib -lssl -lcrypto

/* ssl_client.c
 *
 * Copyright (c) 2000 Sean Walton and Macmillan Publishers.  Use may be in
 * whole or in part in accordance to the General Public License (GPL).
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
*/

/*****************************************************************************/
/*** ssl_client.c                                                          ***/
/***                                                                       ***/
/*** Demonstrate an SSL client.                                            ***/
/*****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define FAIL    -1

/*---------------------------------------------------------------------*/
/*--- OpenConnection - create socket and connect to server.         ---*/
/*---------------------------------------------------------------------*/
int OpenConnection(const char *hostname, int port)
{   int sd;
    struct hostent *host;
    struct sockaddr_in addr;

    if ( (host = gethostbyname(hostname)) == NULL )
    {
        perror(hostname);
        abort();
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}

int
verify(int ok, X509_STORE_CTX *store)
{
    // Always abort verification with an error.
    printf("-------error verify cer\n");
    return 0;
}

/*---------------------------------------------------------------------*/
/*--- InitCTX - initialize the SSL engine.                          ---*/
/*---------------------------------------------------------------------*/
SSL_CTX* InitCTX(void)
{   
    const SSL_METHOD *method;
    SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();		/* Load cryptos, et.al. */
    SSL_load_error_strings();			/* Bring in and register error messages */
    method = TLSv1_2_client_method();		/* Create new client-method instance */
    ctx = SSL_CTX_new(method);			/* Create new context */

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }

    SSL_CTX_use_certificate_file(ctx, "certificate.pem", SSL_FILETYPE_PEM);
    //SSL_CTX_load_verify_locations(ctx, "certificate.pem", NULL);

    // Load trusted CAs from default paths.
    //SSL_CTX_set_default_verify_paths(ctx);

    // Set verify function
    //SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify);

    return ctx;
}

/*---------------------------------------------------------------------*/
/*--- ShowCerts - print out the certificates.                       ---*/
/*---------------------------------------------------------------------*/
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;

    cert = SSL_get_peer_certificate(ssl);	/* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);							/* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);							/* free the malloc'ed string */
        X509_free(cert);					/* free the malloc'ed certificate copy */
    }
    else
        printf("No certificates.\n");
}

/*---------------------------------------------------------------------*/
/*--- main - create SSL context and connect                         ---*/
/*---------------------------------------------------------------------*/
int main(int count, char *strings[])
{   SSL_CTX *ctx;
    int server;
    int res = 1;
    SSL *ssl;
    BIO *web = NULL, *out = NULL;
    char buf[1024];
    int bytes;
    char *hostname, *portnum;

    if ( count != 3 )
    {
        printf("usage: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }
	hostname=strings[1];
	portnum=strings[2];

    ctx = InitCTX();
   
    web = BIO_new_ssl_connect(ctx);
    //std::string  hostn = std::string(hostname)+":"+ std::string(portnum);
    std::string  hostn = "localhost:4242";

    printf("%s\n",hostn.c_str());
    res = BIO_set_conn_hostname(web, hostn.c_str());

    if(!(1 == res)) 
        printf("error conn bio\n");

    BIO_get_ssl(web, &ssl);
    if(!(ssl != NULL)) 
        printf("error cretae ssl");

    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
    res = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS);
    if(!(1 == res)) 
        printf("error cretae preferred cipher");


    res = SSL_set_tlsext_host_name(ssl, hostname);
    if(!(1 == res)) 
        printf("error1\n");

    out = BIO_new_fp(stdout, BIO_NOCLOSE);
    if(!(NULL != out)) 
        printf("error2\n");

    res = BIO_do_connect(web);
    if(!(1 == res)) 
        printf("error3\n");

    res = BIO_do_handshake(web);
    if(!(1 == res)) 
        printf("error4\n");


    X509* cert = SSL_get_peer_certificate(ssl);
    if(cert) { X509_free(cert); } /* Free immediately */
    if(NULL == cert) 
        printf("error5\n");

    /* Step 2: verify the result of chain verification */
    /* Verification performed according to RFC 4158    */
    res = SSL_get_verify_result(ssl);
    if(!(X509_V_OK == res)) 
        printf("error6\n");

    printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
    ShowCerts(ssl); 
    char *msg = "Hello??? send to server aaaaa";
    BIO_write(web, msg, strlen(msg));

    bytes = BIO_read(web, buf, sizeof(buf));    // get reply & decrypt 
    buf[bytes] = 0;
    printf("Received: \"%s\"\n", buf);


    if(out)
        BIO_free(out);

    if(web != NULL)
        BIO_free_all(web);

    if(NULL != ctx)
        SSL_CTX_free(ctx);

    /*
    
    server = OpenConnection(hostname, atoi(portnum));
    ssl = SSL_new(ctx);						

    SSL_set_fd(ssl, server);				// attach the socket descriptor 
    if ( SSL_connect(ssl) == FAIL )			// perform the connection 
        ERR_print_errors_fp(stderr);
    else
    {   char *msg = "Hello??? send to server aaaaa";

        printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);								// get any certs 
        SSL_write(ssl, msg, strlen(msg));			// encrypt & send message 
        bytes = SSL_read(ssl, buf, sizeof(buf));	// get reply & decrypt 
        buf[bytes] = 0;
        printf("Received: \"%s\"\n", buf);
        SSL_free(ssl);								// release connection state 
    }
    */
    
    //SSL_free(ssl);
    close(server);									/* close socket */
    SSL_CTX_free(ctx);								/* release context */
}
