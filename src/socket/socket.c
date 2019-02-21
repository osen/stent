#include "socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BACKLOG 5

struct Socket
{
  int fd;
  vector(unsigned char) buffer;
  ref(struct SocketEvent) event;
  ref(struct SocketEvent) (*pollFunc)(ref(struct Socket), int);
};

struct SocketEvent
{
  int type;
/*
  sstream message;
  sstream reason;
*/
  ref(struct Socket) client;
};

ref(struct Socket) _SocketCreate()
{
  ref(struct Socket) rtn = NULL;

  rtn = salloc(struct Socket);
  _(rtn)->fd = -1;
  _(rtn)->buffer = vector_new(unsigned char);
  _(rtn)->event = salloc(struct SocketEvent);

  return rtn;
}

void _SocketEventClear(ref(struct SocketEvent) ctx)
{
  _(ctx)->type = 0;
  /* TODO: message, reason */
  _(ctx)->client = NULL;
}

ref(struct SocketEvent)
_SocketWaitForListeningEvent(ref(struct Socket) ctx, int t)
{
  ref(struct SocketEvent) rtn = NULL;
  ref(struct Socket) cli = NULL;
  socklen_t sl = 0;
  struct sockaddr_in addr = {0};

  /* Select / Poll socket here */

  rtn = _(ctx)->event;
  _SocketEventClear(rtn);

  cli = _SocketCreate();
  sl = sizeof(addr);

  _(cli)->fd = accept(_(ctx)->fd, (struct sockaddr *)&addr, &sl);

  if(_(cli)->fd == -1)
  {
    SocketClose(cli);
    /* Create event with failure message */
    _(rtn)->type = SOCKET_EVENT_CLOSE;

    return rtn;
  }

  _(rtn)->type = SOCKET_EVENT_CLIENT;
  _(rtn)->client = cli;

  return rtn;
}

ref(struct Socket) SocketListen(int port)
{
  ref(struct Socket) rtn = NULL;
  struct sockaddr_in addr = {0};
  struct sockaddr addr2 = {0};

  rtn = _SocketCreate();
  _(rtn)->fd = socket(AF_INET, SOCK_STREAM, 0);

  if(_(rtn)->fd == -1)
  {
    SocketClose(rtn);

    return NULL;
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if(bind(_(rtn)->fd, (struct sockaddr *)&addr,
    sizeof(addr)) == -1)
  {
    SocketClose(rtn);

    return NULL;
  }

  if(listen(_(rtn)->fd, BACKLOG) == -1)
  {
    SocketClose(rtn);

    return NULL;
  }

  _(rtn)->pollFunc = _SocketWaitForListeningEvent;

  return rtn;
}

void SocketClose(ref(struct Socket) ctx)
{
  if(_(ctx)->fd != -1)
  {
    close(_(ctx)->fd);
  }

  vector_delete(_(ctx)->buffer);
  sfree(_(ctx)->event);
  sfree(ctx);
}

ref(struct SocketEvent) SocketWaitForEvent(ref(struct Socket) ctx, int t)
{
  return _(ctx)->pollFunc(ctx, t);
}

int SocketEventType(ref(struct SocketEvent) ctx)
{
  return _(ctx)->type;
}

ref(struct Socket) SocketEventClient(ref(struct SocketEvent) ctx)
{
  return _(ctx)->client;
}
