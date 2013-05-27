#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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
    char ip[CONNECT_MAX_IP_SIZE];
    char port[CONNECT_MAX_PORT_SIZE];
    get_ip_str((struct sockaddr *)&addr,ip,CONNECT_MAX_IP_SIZE);
    get_port_str((struct sockaddr *)&addr,port,CONNECT_MAX_PORT_SIZE);
    if (isOn(getConfig("verbose","off"))) {
    
        printf("accept client %d, ip %s p:%s\n", cfd,ip,port);
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
    if(c->addr == NULL){
        c->addr = allocMem(CONNECT_MAX_IP_SIZE);
    }
    strncpy(c->addr,ip,CONNECT_MAX_IP_SIZE);
    c->addr[CONNECT_MAX_IP_SIZE-1]='\0';
    if(c->port == NULL ){
        c->port = allocMem(CONNECT_MAX_PORT_SIZE);
    }
    strncpy(c->port,port,CONNECT_MAX_PORT_SIZE);
    c->port[CONNECT_MAX_PORT_SIZE-1]='\0';
    c->tServer = s;
    c->sock.x = c;
    c->sock.f = s->appServer->request;//set request
    c->read = s->appServer->request;
    c->write = s->appServer->response;
    c->sock.fd = cfd;
    c->halfClose=0;
//TODO
    r = sockwant(s->eventQueue,&c->sock, 'r');
    heapinsert(c->tServer->conns,c);
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
    id++;
    ptr->id = id;
    ptr->tServer = NULL;
    ptr->state = state;
    ptr->sock.fd = cfd;
    ptr->sock.f = NULL;
    ptr->sock.added = 0;
    ptr->sock.x = ptr;
    ptr->netMessage = buildNetMessage();
    ptr->offlineTime = -1;
    ptr->addr  = NULL;
    ptr->port = NULL;
    ptr->halfClose =0;
    ptr->read =NULL;
    ptr->write = NULL;
    ptr->user =NULL;
        return ptr;
}

int freeConnect(ConnectPtr *c){
    if(c == NULL){
        return  CONNECT_ERROR_PARAM_ERROR;
    }
    ConnectPtr ptr = (*c);
    ptr->tServer =NULL;
    freeNetMessage(&(ptr->netMessage));
    freeString(&(ptr->addr));
    freeString(&(ptr->port));
    freeMem((void**)&ptr);
    (*c )= NULL;
    return CONNECT_SUCCESS;
}
int getRequestData(ConnectPtr ptr){
	if(ptr == NULL)
	{
		return CONNECT_ERROR_PARAM_ERROR;
	}
    char buf[CONNECT_RECV_BUF_SIZE];
    int recvlength=0;
    int result =CONNECT_SUCCESS;
    if(isOn(getConfig("verbose","off"))){
        printf("read data\n" );
    }
    while(1){
        recvlength =read(ptr->sock.fd,buf,CONNECT_RECV_BUF_SIZE);
        if(recvlength == -1){
            checkConnectError(ptr," read  data failed");
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
    int needWrite =0;
    if(result == PARSER_ERROR_DATA_NEED_MORE){
        if(isOn(getConfig("verbose","off"))){
            printf("need more data\n" );
        }
        ptr->state = CONNECT_STATE_READ_WAIT_DATA;//等待继续读取数据
    }
    else if(result == PARSER_ERROR_FORMAT_ERROR){
        if(isOn(getConfig("verbose","off"))){
            printf(" data format error\n" );
        }
        ptr->state = CONNECT_STATE_READ_FORMAT_ERROR;
        needWrite =1;
    }else if(result == PARSER_SUCCESS){
        if(ptr->netMessage->readState == NETMESSAGE_READSTATE_FINISH){

            if(isOn(getConfig("verbose","off"))){
                printf("message ready data\n" );
            }
            ptr->state = CONNECT_STATE_READ_MESSAGE_READY;
            needWrite = 1;   
        }else{
            if(isOn(getConfig("verbose","off"))){
                printf("?\n" );
            }
        }
    }
    if(needWrite){

        int r = sockwant(ptr->tServer->eventQueue,&(ptr->sock),'w');
        ptr->sock.f= ptr->write;
        if (r == -1) {
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"sockwant failed");

        }
    }
    return result;
	
}

int setConnectHostInfo(ConnectPtr ptr,const char * addr,const char *port){
    if(ptr == NULL || addr ==NULL || port == NULL){
        return CONNECT_ERROR_PARAM_ERROR;
    }
    if(ptr->addr ==NULL){
        ptr->addr = allocMem(CONNECT_MAX_IP_SIZE);
    }
    strncpy(ptr->addr,addr,CONNECT_MAX_IP_SIZE);   
    ptr->addr[CONNECT_MAX_IP_SIZE-1]='\0';
    if(ptr->port ==NULL){
        ptr->port = allocMem(CONNECT_MAX_PORT_SIZE);
    }
    strncpy(ptr->port,port,CONNECT_MAX_PORT_SIZE);
    ptr->port[CONNECT_MAX_PORT_SIZE-1]='\0';
    setNetMessageParam(ptr->netMessage,"host",addr);
    setNetMessageParam(ptr->netMessage,"port",port);
    return CONNECT_SUCCESS;
}  
int setSecureInfo(ConnectPtr ptr,UserPtr uptr){
    if(ptr == NULL || uptr==NULL){
        return CONNECT_ERROR_PARAM_ERROR;
    }
    return setNetMessageSendUser(ptr->netMessage,uptr);
} 
void checkConnectError(ConnectPtr c, const char *s)
{
    if (errno == EAGAIN) return;
    if (errno == EINTR) return;
    if (errno == EWOULDBLOCK) return;

    addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"error happened on connect %s:%s,%s",c->addr,c->port,s);
    connectClose(c);
    return;
}

int sendResponseData(ConnectPtr ptr){
    //may need check ev 
    if(ptr == NULL){
        return CONNECT_ERROR_PARAM_ERROR;
    }
    NetMessagePtr mptr = ptr->netMessage;

    if(mptr->sendState == NETMESSAGE_WRITESTATE_FINISH){
        return CONNECT_SUCCESS;
    }
    if(mptr->sendState == NETMESSAGE_WRITESTATE_WAIT){
        reparserNetMessage(ptr->netMessage);
        mptr->sendState = NETMESSAGE_WRITESTATE_WRITING;
        if(isOn(getConfig("verbose","off"))){
            printf("send  data\n" );
        } 
    }
    if(mptr->sendState == NETMESSAGE_WRITESTATE_WRITING){
        
        int writeLength = write(ptr->sock.fd,mptr->sendBuf+mptr->writedLength,mptr->sendBufLength-mptr->writedLength);
        if(writeLength <0 ){
            printf("w%d,wd%d\n",writeLength,mptr->writedLength);
            checkConnectError(ptr,"send Data failed");
            return CONNECT_SUCCESS;
        }
        mptr->writedLength += writeLength;
        if(mptr->writedLength == mptr->sendBufLength){
            mptr->sendState = NETMESSAGE_WRITESTATE_FINISH;
            if(isOn(getConfig("verbose","off"))){
                printf("send  data success\n" );
            }
            int r = sockwant(ptr->tServer->eventQueue,&(ptr->sock),'r');
            ptr->sock.f = ptr->read;
            if (r == -1) {
                addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"sockwant failed");

            }
        }  
    }
    
    return CONNECT_SUCCESS;
}
int connectClose(ConnectPtr ptr){
    //when  connet remove  
    if(ptr == NULL){
        return CONNECT_ERROR_PARAM_ERROR;
    }
    sockwant(ptr->tServer->eventQueue,&ptr->sock, 0);
    closeFd(ptr->sock.fd);
    //TODO
}
int compareConnect(ConnectPtr con1,ConnectPtr con2){
    if(con1 == NULL || con2 == NULL){
        return 1;
    }else{
        return con1->offlineTime>con2->offlineTime;
    }
}

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    s, maxlen);
            break;


        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, maxlen);
            break;


        default:
            strncpy(s, "Unknown AF", maxlen);
            
            return NULL;
    }
    return s;
}
char *get_port_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch(sa->sa_family) {
        case AF_INET:
            snprintf(s,maxlen,"%d", ntohs(((struct sockaddr_in *)sa)->sin_port));
            
            break;


        case AF_INET6:
            snprintf(s,maxlen,"%d" ,ntohs(((struct sockaddr_in6 *)sa)->sin6_port));
            break;


        default:
            strncpy(s, "-1", maxlen);
            return NULL;
    }
    return s;
}
ConnectPtr buildRequestConnect(TransfarServerPtr ptr,const char* addr,const char * port,Handle read,Handle write){
    if(ptr == NULL || addr == NULL || port == NULL){
        return NULL;
    }
    int fd = makeClientSocket(addr,port);
    ConnectPtr cPtr =buildConnect(fd,CONNECT_STATE_SEND_CONNECTING);
    cPtr->tServer = ptr;
    cPtr->addr = allocString(addr);
    cPtr->port = allocString(port);
    cPtr->read = read;
    cPtr->write =write;
    int r = sockwant(ptr->eventQueue,&cPtr->sock, 'w');
    cPtr->sock.f = cPtr->write;
    heapinsert(ptr->conns,cPtr);
    if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_CONNECT_POSITION_NAME,"sockwant failed");
    }

    return cPtr;
}

int tickConnects(TransfarServerPtr ptr){

}
