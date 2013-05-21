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
#include "../parser/net_message.h"
#include "../parser/parser.h"

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
    if (isOn(getConfig("verbose","off"))) {
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

    c = buildConnect(cfd,CONNECT_STATE_READ_WAIT_DATA);
    if (!c) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"buildConnect failed %d",cfd);
        closeFd(cfd);
        return;
    }
    c->tServer = s;
    c->sock.x = c;
   // c->sock.f = (Handle)prothandle;
    c->sock.fd = cfd;

    r = sockwant(&c->sock, 'r');
    if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"sockwant failed");
        closeFd(cfd);
        return;
    }
}
void closeFd(int fd){
    close(fd);
    if (isOn(getConfig("verbose","off"))) {
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
    ConnectPtr ptr = (ConnectPtr)allocMem(sizeof(Connect));
    if(ptr == NULL){
        return NULL;
    }
    ptr->id = id;
    ptr->tServer = NULL;
    ptr->state = state;
    ptr->netMessage = buildNetMessage();
    ptr->offlineTime = -1;
    ptr->addr  = NULL;
    ptr->port = NULL;
    return ptr;
}

int freeConnect(ConnectPtr *c){
    if(c == NULL){
        return  CONNECT_ERROR_PARAM_ERROR;
    }
    return CONNECT_SUCCESS;
}
int getRequestData(ConnectPtr ptr){
	if(ptr == NULL)
	{
		return CONNECT_ERROR_PARAM_ERROR;
	}
    char buf[CONNECT_RECV_BUF_SIZE];
    int recvlength=0;
    int result =0;
    while(1){
        recvlength =read(ptr->sock.fd,buf,CONNECT_RECV_BUF_SIZE);
        if(recvlength == -1){
            checkConnectError(ptr," read failed");
            break;
        }else if(recvlength == 0){
            //end 
            break;
        }else{
            result = parserNetMessage(ptr->netMessage,buf,recvlength);
            if(result == PARSER_ERROR_DATA_NEED_MORE){
                continue;
            }else{
                break;
            }
        }
    }
    if(result == PARSER_ERROR_DATA_NEED_MORE){
        ptr->state = CONNECT_STATE_READ_WAIT_DATA;//等待继续读取数据
    }
    else if(result == PARSER_ERROR_FORMAT_ERROR){
        ptr->state = CONNECT_STATE_READ_FORMAT_ERROR;
    }else if(result == PARSER_SUCCESS){
        ptr->state = CONNECT_STATE_READ_MESSAGE_READY;
    }
    return CONNECT_SUCCESS;
	
}
void checkConnectError(ConnectPtr c, const char *s)
{
    if (errno == EAGAIN) return;
    if (errno == EINTR) return;
    if (errno == EWOULDBLOCK) return;

    addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"error happened on connect :%s",s);
    connectClose(c);
    return;
}
int connectClose(ConnectPtr ptr){
    // do clean
}
int compareConnect(ConnectPtr con1,ConnectPtr con2){
    if(con1 == NULL || con2 == NULL){
        return 1;
    }else{
        return con1->offlineTime>con2->offlineTime;
    }
}