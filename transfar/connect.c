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
#include "../util/log.h"
#include "connect.h"

void acceptConnect(TransfarServerPtr s,const int ev )  
{
    int fd = s->sock.fd;
    ConnectPtr c;
    int cfd, flags, r;
    socklen_t addrlen;
    struct sockaddr_in6 addr;

    addrlen = sizeof addr;
    //接受连接
    cfd = accept(fd, (struct sockaddr *)&addr, &addrlen);
    if (cfd == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK){
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"accept client failed");
        }
        return;
    }
    if (isOn(getConfig("verbose"))) {
        printf("accept client %d\n", cfd);
    }

    flags = fcntl(cfd, F_GETFL, 0);
    if (flags < 0) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"get fileflags failed");
        closeFd(cfd);
        return;
    }

    r = fcntl(cfd, F_SETFL, flags | O_NONBLOCK);
    if (r < 0) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"setting O_NONBLOCK failed");
        closeFd(cfd);
        return;
    }

    c = buildConnect(cfd,CONNECT_STATE_WANTDATA);
    if (!c) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"buildConnect failed %d",cfd);
        closeFd(cfd);
        return;
    }
    c->srv = s;
    c->sock.x = c;
   // c->sock.f = (Handle)prothandle;
    c->sock.fd = cfd;

    r = sockwant(&c->sock, 'r');
    if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"sockwant failed");
        close(cfd);
        return;
    }
}
void closeFd(int fd){
    close(fd);
    if (isOn(getConfig("verbose"))) {
        printf("close %d\n", fd);
    }
    return ;
}

//新建连接对象
ConnectPtr buildConnect(const int cfd, const int state){
    if(cfd<0 ){
        return NULL;
    }
    static int id;
    ConnectPtr ptr = malloc(sizeof(Connect));
    ptr->id = id;
    ptr->srv = NULL;
    ptr->state = state;
    ptr->NetMEssagePtr = NM_buildNetMessage();
    ptr->onlineTime = -1;
    ptr->ip  = NULL;
    return ptr;
}

int freeConnect(ConnectPtr c){
    if(c == NULL){
        return  CONNECT_ERROR_PARAM_ERROR;
    }
    return CONNECT_SUCCESS;
}