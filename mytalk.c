#include <arpa/inet.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <ncurses.h>
#include <poll.h>

#define MAX_CONNECTIONS 100
#define DEFAULT_BACKLOG 100
#define MAX_CONNECTIONSV 1
#define MAXLINE 1024

void chat   (int sockfd);

int main(int argc, char *argv[]) {
    int sockfd;     
    struct sockaddr_in sa;
    struct hostent *hostent;
    int port;
    char *hostname = "localhost";

    if (argc == 2) {
        port = atoi(argv[1]);
        if (port < 1 || port > 65535) {
            printf("Invalid port");
        }    
        hostent = gethostbyname(hostname);

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        sa.sin_family = AF_INET;
        sa.sin_port = htons(port); 
        sa.sin addr.s_addr = *(uint32_t*)hostent−>h_addr_list[0]; 
        bind(sockfd, (struct sockaddr*)&sa, sizeof(sa));
        listen(sockfd, DEFAULT_BACKLOG);
        
        chat(sockfd);

        close(sockfd);
        return 0;
    } else {
        port = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        sa.sin_family = AF_INET;
        sa.sin_port = htons(port); 
        sa.sin_addr.s_addr = htonl(INADDR_ANY);

        connect(sockfd, (struct sockaddr*)&sa, sizeof(sa));
        chat(sockfd);
    }

}

void chat(int listener) {
    int i, j, done, mlen, newsock, num = 0;
    struct sockaddr_in peerinfo;
    socklen_t slen;
    char buff[MAXLINE], addr[INET_ADDRSTRLEN];
    struct pollfd fds[MAX_CONNECTIONS + 1];
    done = 0;
    buff[0] = ’\0’; /* Make sure this is a string for strncmp below */
    fds[0].fd = listener;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    num = 1;
    do {
        poll(fds, num, −1);
        /* check for connections */
        if ((fds[0].revents & POLLIN) && (num <= MAX_CONNECTIONS)) {
            /* accept a new connection and add the client to the list */
            slen = sizeof(peerinfo);
            newsock = accept(listener, (struct sockaddr *)&peerinfo, &slen);
            inet_ntop(AF INET, &peerinfo.sin addr.s addr, addr, sizeof(addr));
            printf("New connection from: %s:%d\n", addr, htons(peerinfo.sin_port));
            fds[num].fd = newsock;
            fds[num].events = POLLIN;
            fds[num].revents = 0;
            num++;
        }
        /* check for data */
        for (i = 1; i < num; i++) {
            if (fds[i].revents & POLLIN) {
            /* read from this client and broadcast to all */
            slen = sizeof(struct sockaddr in);
            mlen = recv(fds[i].fd, buff, sizeof(buff), 0);
            for (j = 1; j < num; j++) /* broadcast to everyone else*/
            if (i != j) /* not self */
            send(fds[j].fd, buff, mlen, 0);
            }
        }
        if (!strncmp(buff, "bye", 3)) {
            done = 1;
        }
    } while (!done);
}