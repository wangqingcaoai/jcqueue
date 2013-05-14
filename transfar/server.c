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


int
makeServerSocket(char *host, char *port)
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
          char hbuf[NI_MAXHOST], pbuf[NI_MAXSERV], *h = host, *p = port;
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
        continue;
      }
//监听，设置连接数
      r = listen(fd, 1024);
      if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"listen()");
        close(fd);
        continue;
      }

      break;
    }

    freeaddrinfo(airoot);

    if(ai == NULL)
      fd = -1;

    return fd;
}


void
srvserve(Server *s)
{
    int r;
    Socket *sock;
    int64 period = 10*1000000; // 10ms

    if (sockinit() == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sockinit");
        exit(1);
    }

    s->sock.x = s;
    s->sock.f = (Handle)srvaccept;
    // s->conns.less = (Less)connless;
    // s->conns.rec = (Record)connrec;

    r = listen(s->sock.fd, 1024);
    if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"listen");
        return;
    }

    r = sockwant(&s->sock, 'r');
    if (r == -1) {
        addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"sockwant");
        printf("%d %s\n",errno ,strerror(errno));
        exit(2);
    }


    int64 t = nanoseconds();
    for (;;) {
      //循环处理
        int rw = socknext(&sock, period);
        if (rw == -1) {
            addLog(LOG_WARNING,LOG_LAYER_TRANSFAR,TRANSFAR_SERVER_POSITION_NAME,"socknext");
            exit(1);
        }

        int64 t1 = nanoseconds();
        if (t1-t > period) {
            srvtick(s);
            t = t1;
        }

        if (rw) {
            sock->f(sock->x, rw);
        }
    }
}
/**
 * 连接接入处理
 */
void
srvaccept(Server *s, int ev)
{
    acceptConnect(s,ev);    
}
/**
 * 心跳处理
 */
void srvtick(Server *s){

}