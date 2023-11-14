//Server side Program in C/C++

#include <stdint.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


static void msg( const char* msg){
    fprintf(stderr, "%s\n", msg);
}

static void die(const char* msg){
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err,msg);
    abort();
}

static void do_something(int connfd){
    char rbuf[64] = {};

    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);

    if( n < 0 ){
        msg("read() error");
        return;
    }

    printf("Client says: %s\n",rbuf);

    char wbuf[] = "world";
    write(connfd , wbuf, strlen(wbuf));
}


int main(){
    //syscall to return integer for socket communication from kernel
    int fd = socket(AF_INET, SOCK_STREAM , 0);
    if( fd < 0 ){
        die("socket()");
    }

    //Set the option in various layer which will establish communication.
    int val = 1;
    setsockopt(fd, SOL_SOCKET , SO_REUSEADDR, &val , sizeof(val));

    //bind, dealing wih IPv4 addresses
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);// wildcard address 0.0.0.0
    int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));

    if(rv){
        die("bind()");
    }

    //listen
    rv = listen(fd , SOMAXCONN);

    if( rv ){
        die("listen()");
    }

    //Looping to connect to each connection.
    while(true){
        //accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof( client_addr );
        int connfd = accept(fd , (struct sockaddr*)& client_addr, &socklen);
        if( connfd < 0 ){
            //error
            printf("Error encountered while connecting with client.");
        }

        do_something(connfd);
        close(connfd);
    }

    return 0;
}
