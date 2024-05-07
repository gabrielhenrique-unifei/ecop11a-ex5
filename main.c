#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static const int PORT = 8080;
static const char *HTML_RESPONSE_TEMPLATE =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n\r\n"
    "<!DOCTYPE html>\n"
    "<html><head><title>My Simple Web Server</title></head>\n"
    "<body><h1>FUCK YOU!</h1></body></html>\r\n";

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) < 0) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d...\n", PORT);

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (new_sockfd < 0) {
            perror("accept");
            continue;
        }

        char buffer[1024];
        ssize_t n = read(new_sockfd, buffer, sizeof(buffer));
        if (n > 0) {
            printf("Received request:\n%s\n", buffer);

            // Send the HTML response
            write(new_sockfd, HTML_RESPONSE_TEMPLATE, strlen(HTML_RESPONSE_TEMPLATE));
        }

        close(new_sockfd);
    }

    close(sockfd);
    return 0;
}