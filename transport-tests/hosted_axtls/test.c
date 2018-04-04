#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <axTLS/ssl.h>

int main(){
    int socket_fd;
    struct sockaddr_in ip;
    struct hostent *host = gethostbyname("www.google.com");
    assert(host != NULL);

    ip.sin_family = AF_INET;
    ip.sin_port = 443;

    memcpy(&ip.sin_addr.s_addr, host->h_addr_list[0], sizeof host->h_addr_list[0]);

    socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    connect(socket_fd, (struct sockaddr *) &ip, sizeof ip);

    SSL_CTX *ssl_ctx = ssl_ctx_new(SSL_SERVER_VERIFY_LATER, 0);

    SSL *ssl = ssl_client_new(ssl_ctx, socket_fd, NULL, 0, NULL);

    printf("Handshake: %d, Verify: %d\n",
           ssl_handshake_status(ssl),
           ssl_verify_cert(ssl));

    printf("DN: %s\n", ssl_get_cert_dn(ssl, 0));

    ssl_free(ssl);

    ssl_ctx_free(ssl_ctx);
}
