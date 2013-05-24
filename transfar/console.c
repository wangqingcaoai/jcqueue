#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "console.h"
#include <unistd.h>
#include "../util/log.h"
#include "../util/util.h"
#include "../parser/parser.h"
#include <fcntl.h>
#include <errno.h>
ConsolePtr buildConsole(TransfarServerPtr tServer){
    if(tServer == NULL){
        return NULL;
    }
    ConsolePtr ptr = allocMem(sizeof(Console));
    if(ptr == NULL){
        return NULL;
    }
    ptr->in.fd= STDIN_FILENO;
    ptr->tServer = tServer;
    ptr->in.x = ptr;
    ptr->in.f = tServer->appServer->consoleIn;
    ptr->out.fd = STDOUT_FILENO;
    ptr->out.x = ptr;
    ptr->out.f = tServer->appServer->consoleOut;
    ptr->state = CONSOLE_STATE_READ_WAIT_DATA;
    ptr->netMessage = buildNetMessage();
    ptr->netMessage->protocolType = NETMESSAGE_TYPE_CMD;
    ptr->netMessage->version = NETMESSAGE_VERSION_CMD;
    //SET USER
    setOnBlock(STDOUT_FILENO);
    setOnBlock(STDIN_FILENO);
    return ptr;
}
int freeConsole(ConsolePtr *pptr){
    if(pptr == NULL){
        return 1;
    }
    ConsolePtr ptr = (*pptr);
    if(ptr == NULL){
        return 1;
    }
    freeMem((void**)&ptr);
    (*pptr) = NULL;
}
int setOnBlock(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"getting  flags failed");
        return -1;
    }

    int r = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"setting O_NONBLOCK failed");
        return -1;
    }
    return 0;
}

int getConsoleData(ConsolePtr ptr){
    if(ptr == NULL)
    {
        return CONSOLE_ERROR_PARAM_ERROR;
    }
    char buf[CONSOLE_RECV_BUF_SIZE];
    int recvlength=0;
    int result =CONSOLE_SUCCESS;
    if(isOn(getConfig("verbose","off"))){
        printf("read console data\n" );
    }
    while(1){
        recvlength =read(ptr->in.fd,buf,CONSOLE_RECV_BUF_SIZE);
        if(recvlength == -1){
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
        if(isOn(getConfig("verbose","off"))){
            printf("need more console data\n" );
        }
        ptr->state = CONSOLE_STATE_READ_WAIT_DATA;//等待继续读取数据
    }
    else if(result == PARSER_ERROR_FORMAT_ERROR){
        if(isOn(getConfig("verbose","off"))){
            printf(" data format error\n" );
        }
        ptr->state = CONSOLE_STATE_READ_FORMAT_ERROR;
    }else if(result == PARSER_SUCCESS){
        if(isOn(getConfig("verbose","off"))){
            printf("message ready data\n" );
        }
        ptr->state = CONSOLE_STATE_READ_MESSAGE_READY;
    }
    return result;
    
}

int sendConsoleData(ConsolePtr ptr){
    //may need check ev 
    if(ptr == NULL){
        return CONSOLE_ERROR_PARAM_ERROR;
    }
    
    NetMessagePtr mptr = ptr->netMessage;
    if(mptr->sendState == NETMESSAGE_WRITESTATE_FINISH){
        return CONSOLE_SUCCESS;
    }
    if(!(mptr->sendBuf !=NULL && mptr->sendBufLength > 0)){
        reparserNetMessage(ptr->netMessage);
    }
    int writeLength = write(ptr->out.fd,mptr->sendBuf+mptr->writedLength,mptr->sendBufLength-mptr->writedLength);
    if(writeLength <0 ){
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"send data failed");
        return CONNECT_SUCCESS;
    }
    mptr->writedLength += writeLength;
    if(mptr->writedLength == mptr->sendBufLength){
        freeMem((void**)&mptr->sendBuf);
        mptr->writedLength = 0;
        mptr->sendState = NETMESSAGE_WRITESTATE_FINISH;
    }else{
        mptr->sendState = NETMESSAGE_WRITESTATE_WRITING;
    }
    return CONSOLE_SUCCESS;
}
int consoleStart(TransfarServerPtr ptr){
    if(ptr == NULL ){
        return CONSOLE_ERROR_PARAM_ERROR;
    }
    if(ptr->eventQueue <0){
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"eventQueue id is not right");
                return;
    }
    int r;
    r = sockwant(ptr->eventQueue,&(ptr->console->in), 'r');
    if (r == -1) {
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sockwant");
            printf("%d %s\n",errno ,strerror(errno));
            exit(2);
    }

    // r = sockwant(ptr->eventQueue,&(ptr->console->out), 'w');
    // if (r == -1) {
    //         addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sockwant");
    //         printf("%d %s\n",errno ,strerror(errno));
    //         exit(2);
    // }
    return CONSOLE_SUCCESS;
}