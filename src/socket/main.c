#include "socket.h"

#include <stent.h>
#include <stdio.h>

int main()
{
  ref(Socket) s = NULL;
  ref(SocketEvent) se = NULL;
  ref(Socket) c = NULL;

  s = SocketListen(1987);

  if(!s)
  {
    printf("Failed to open listening socket\n");
    return 1;
  }

  while(1)
  {
    se = SocketWaitForEvent(s, -1);
    if(!se) continue;

    if(SocketEventType(se) == SOCKET_EVENT_CLIENT)
    {
      printf("Client connected\n");
      c = SocketEventClient(se);
      SocketClose(c);
    }
    else if(SocketEventType(se) == SOCKET_EVENT_CLOSE)
    {
      printf("Listen socket closed\n");
    }
  }

  SocketClose(s);

  return 0;
}
