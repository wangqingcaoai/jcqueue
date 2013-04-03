#include <stdio.h>
#include "transfar/server.h"
#include "util/config.h"
int main(int argc, char const *argv[])
{
    initConfig();
    Server s;
    s.user=NULL,
    s.addr = "127.0.0.1";
    s.port = "11220";
    s.sock.added = 0;
    s.sock.x =NULL;

    int fd = makeServerSocket(s.addr,s.port);
    s.sock.fd = fd;
    srvserve(&s);
    return 0;
}
