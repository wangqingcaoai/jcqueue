#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdlib.h>
#include "../util/type.h"
#include "sockets.h"
#include "../util/log.h"
#include "sd-daemon.h"
#include "server.h"
#include "connect.h"
#include "console.h"

TransfarServerPtr buildTransfarServer(AppServerPtr app,const char* addr,const char* port){
    
    if(app == NULL){
        return NULL;
    }
    if(isEmptyString(addr) || isEmptyString(port) ){
        return NULL;
    }
    static int id;
    
    TransfarServerPtr ptr = (TransfarServerPtr)allocMem(sizeof(TransfarServer));
    if(ptr==NULL){
        return ptr;
    }
    id++;
    ptr->id = id;
    ptr->appServer = app;
    ptr->addr = allocString(addr);
    ptr->port = allocString(port);
    ptr->eventQueue = sockinit();//TODO need change
    if (ptr->eventQueue == -1) {
        addLog(LOG_ERROR,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sockinit");
        exit(1);
    }
    ptr->sock.fd = makeServerSocket(addr,port);
    ptr->sock.x = ptr;
    ptr->sock.f = (Handle)srvaccept;
    ptr->conns = buildHeap(NULL,(Less)compareConnect);
    ptr->console = buildConsole(ptr);
    return ptr;
}

int freeTransfarServer(TransfarServerPtr *pptr){
    if(pptr == NULL){
        return TRANSFAR_SERVER_ERROR_PARAM_ERROR;
    }
    TransfarServerPtr ptr = (*pptr);
    if(ptr == NULL){
        return TRANSFAR_SERVER_ERROR_PARAM_ERROR;
    }
    freeString(&(ptr->addr));
    freeString(&(ptr->port));
    if(ptr->sock.fd!=-1){
        close(ptr->sock.fd);
    }
    freeHeap(&(ptr->conns),(Free)freeConnect);
    freeConsole(&(ptr->console));
    free(ptr);
    ptr = NULL;
    (*pptr) = NULL;
    return TRANSFAR_SERVER_SUCCESS;
}


int
makeServerSocket(const char *host,const char *port)
{
        int fd = -1, flags, r;
        struct linger linger = {0, 0};
        struct addrinfo *airoot, *ai, hints;

        /* See if we got a listen fd from systemd. If so, all socket options etc
         * are already set, so we check that the fd is a TCP listen socket and
         * return. */
        r = sd_listen_fds(1);
        if (r < 0) {
                return addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sd_listen_fds"), -1;
        }
        if (r > 0) {
                if (r > 1) {
                        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"inherited more than one listen socket;ignoring all but the first");
                        r = 1;
                }
                fd = SD_LISTEN_FDS_START;
                r = sd_is_socket_inet(fd, 0, SOCK_STREAM, 1, 0);
                if (r < 0) {
                        errno = -r;
                        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sd_is_socket_inet");
                        return -1;
                }
                if (!r) {
                        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"inherited fd is not a TCP listen socket");
                        return -1;
                }
                return fd;
        }
//初始化数据
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        r = getaddrinfo(host, port, &hints, &airoot);
        if (r == -1)
            return addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"getaddrinfo()"), -1;

        for(ai = airoot; ai; ai = ai->ai_next) {
            //对socket进行设置
            fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (fd == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"socket()");
                continue;
            }

            flags = fcntl(fd, F_GETFL, 0);
            if (flags < 0) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"getting flags");
                close(fd);
                continue;
            }

            r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting O_NONBLOCK");
                close(fd);
                continue;
            }

            flags = 1;
            r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof flags);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting SO_REUSEADDR on fd %d", fd);
                close(fd);
                continue;
            }
            r = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof flags);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting SO_KEEPALIVE on fd %d", fd);
                close(fd);
                continue;
            }
            r = setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger, sizeof linger);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting SO_LINGER on fd %d", fd);
                close(fd);
                continue;
            }
            r = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof flags);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting TCP_NODELAY on fd %d", fd);
                close(fd);
                continue;
            }

            if (isOn(getConfig("verbose"))) {
                    char hbuf[NI_MAXHOST], pbuf[NI_MAXSERV];
                    const char *h = host, *p = port;
                    r = getnameinfo(ai->ai_addr, ai->ai_addrlen,
                                    hbuf, sizeof hbuf,
                                    pbuf, sizeof pbuf,
                                    NI_NUMERICHOST|NI_NUMERICSERV);
                    if (!r) {
                            h = hbuf;
                            p = pbuf;
                    }
                    if (ai->ai_family == AF_INET6) {
                            printf("bind %d [%s]:%s\n", fd, h, p);
                    } else {
                            printf("bind %d %s:%s\n", fd, h, p);
                    }
            }
            //绑定socket
            r = bind(fd, ai->ai_addr, ai->ai_addrlen);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"bind()");
                close(fd);
                exit(1);
            }
//监听，设置连接数
            r = listen(fd, 1024);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"listen()");
                close(fd);
                continue;
                exit(1);
            }

            break;
        }

        freeaddrinfo(airoot);

        if(ai == NULL)
            fd = -1;

        return fd;
}


void
srvStart(TransfarServerPtr s)
{
        int r;
   
        r = listen(s->sock.fd, 1024);
        if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"listen");
                return;
        }
        if(s->eventQueue <0){
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"eventQueue id is not right");
                return;
        }
        r = sockwant(s->eventQueue,&s->sock, 'r');
        if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sockwant");
                printf("%d %s\n",errno ,strerror(errno));
                exit(2);
        }
        
}
void srvProcess(TransfarServerPtr s ){
    if(s == NULL){
        return ;
    }
    if(s->eventQueue <0){
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"eventQueue id is not right");
                return;
    }
    Socket *sock;
    s->lastServerTime = nanoseconds();
    int rw = socknext(s->eventQueue ,&sock, s->period);
    if (rw == -1) {
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"socknext");
            exit(1);
    }

    int64 t1 = nanoseconds();
    if (t1-s->lastServerTime > s->period) {
        if(s->appServer == NULL){
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"appServer   is empty ");
            exit(1);
        }
        if(s->appServer->tick!=NULL){

            s->appServer->tick(s->appServer);
            s->lastServerTime = t1;
        }else{
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"appServer tick method is empty ");
            exit(1);
        }
            
    }

    if (rw) {
        if(sock->f!=NULL){
            sock->f(sock->x, rw);
        }else{
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"socket handle is empty");
            exit(1);
        }
            
    }
}

/**
 * 连接接入处理
 */
void
srvaccept(TransfarServerPtr s, int ev)
{
    acceptConnect(s,ev);    
}
/**
 * 心跳处理
 */
void srvtick(TransfarServerPtr s){
    if(s == NULL){
        return ;
    }
    //

}
int makeClientSocket(const char* host,const char*port){
        int fd = -1, flags, r;
        struct linger linger = {0, 0};
        struct addrinfo *airoot, *ai, hints;

        /* See if we got a listen fd from systemd. If so, all socket options etc
         * are already set, so we check that the fd is a TCP listen socket and
         * return. */
//初始化数据
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        r = getaddrinfo(host, port, &hints, &airoot);
        if (r == -1)
            return addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"getaddrinfo()"), -1;

        for(ai = airoot; ai; ai = ai->ai_next) {
            //对socket进行设置
            fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (fd == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"socket()");
                continue;
            }

            flags = fcntl(fd, F_GETFL, 0);
            if (flags < 0) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"getting flags");
                close(fd);
                continue;
            }

            r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting O_NONBLOCK");
                close(fd);
                continue;
            }

            flags = 1;
            r = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flags, sizeof flags);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting SO_REUSEADDR on fd %d", fd);
                close(fd);
                continue;
            }
            r = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof flags);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting SO_KEEPALIVE on fd %d", fd);
                close(fd);
                continue;
            }
            r = setsockopt(fd, SOL_SOCKET, SO_LINGER, &linger, sizeof linger);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting SO_LINGER on fd %d", fd);
                close(fd);
                continue;
            }
            r = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof flags);
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting TCP_NODELAY on fd %d", fd);
                close(fd);
                continue;
            }

            if (isOn(getConfig("verbose"))) {
                    char hbuf[NI_MAXHOST], pbuf[NI_MAXSERV];
                    const char *h = host, *p = port;
                    r = getnameinfo(ai->ai_addr, ai->ai_addrlen,
                                    hbuf, sizeof hbuf,
                                    pbuf, sizeof pbuf,
                                    NI_NUMERICHOST|NI_NUMERICSERV);
                    if (!r) {
                            h = hbuf;
                            p = pbuf;
                    }
                    if (ai->ai_family == AF_INET6) {
                            printf("connect %d [%s]:%s\n", fd, h, p);
                    } else {
                            printf("connect %d %s:%s\n", fd, h, p);
                    }
            }
            //socket
            r = connect(fd, ai->ai_addr, ai->ai_addrlen);
            break;
        }

        freeaddrinfo(airoot);

        if(ai == NULL){
                fd = -1;
        }

        return fd;

}
int tickTransfarServer(TransfarServerPtr ptr){
    if(ptr == NULL){
        return TRANSFAR_SERVER_ERROR_PARAM_ERROR;
    }
    srvtick(ptr);
    return TRANSFAR_SERVER_SUCCESS;
}

int stopTransfarServer(TransfarServerPtr ptr){
    
    if(ptr == NULL){
        return TRANSFAR_SERVER_ERROR_PARAM_ERROR;
    }
    if(isOn(getConfig("verbose","off"))){
        printf("stopTransfarServer ...\n");
    }
    
    sockwant(ptr->eventQueue,&ptr->sock, 0);
    closeFd(ptr->sock.fd);
    ConnectPtr con;
    int count = getHeapLength(ptr->conns),i;
    for ( i = 0; i < count; ++i)
    {
        con = getHeapDataByIndex(ptr->conns,i);
        connectClose(con);
    }    
}