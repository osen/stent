#include <stent.h>

#define SOCKET_EVENT_CLOSE 1
#define SOCKET_EVENT_CONNECTED 2
#define SOCKET_EVENT_MESSAGE 3
#define SOCKET_EVENT_CLIENT 4

struct Socket;
struct SocketEvent;

void SocketClose(ref(struct Socket) ctx);
ref(struct SocketEvent) SocketWaitForEvent(ref(struct Socket) ctx, int t);

ref(struct Socket) SocketListen(int port);

int SocketEventType(ref(struct SocketEvent) ctx);
ref(struct Socket) SocketEventClient(ref(struct SocketEvent) ctx);

typedef struct Socket Socket;
typedef struct SocketEvent SocketEvent;


















#ifdef ASASASAS

struct Socket
{
  ref(int) fd;
  vector(unsigned char) buffer;
  ref(struct SocketEvent) event;
  /* ref(struct SocketEvent) events; */
  ref(struct SocketEvent) (*pollFunc)(ref(struct Socket), int);
};

struct SocketEvent
{
  int type;
  sstream message;
  sstream reason;
  ref(struct Socket) client;
  /* ref(struct SocketEvent) next; */
};

int SocketEventType(ref(struct SocketEvent) e);
sstream SocketEventReason(ref(struct SocketEvent) e);
sstream SocketEventMessage(ref(struct SocketEvent) e);
ref(struct Socket) SocketEventClient(ref(struct SocketEvent) e);

void SocketClose(ref(struct Socket) s);
ref(struct SocketEvent) SocketPollForEvent(ref(struct Socket) s);
ref(struct SocketEvent) SocketWaitForEvent(ref(struct Socket) s, int t);

ref(struct Socket) SocketListen(int port);

void SocketSendMessage(ref(struct Socket) s, sstream m);

/*

ref(SocketEvent) e = NULL;
sstream t = NULL;

while(1)
{
  e = SocketPollForEvent(s);
  if(!e) continue;

  if(SocketEventType(e) == SOCKET_EVENT_CONNECTED)
  {
    printf("Socket connected!\n");
  }
  else if(SocketEventType(e) == SOCKET_EVENT_MESSAGE)
  {
    t = SocketEventMessage(e);
    printf("Received: %s\n", sstream_cstr(t));
  }
  else if(SocketEventType(e) == SOCKET_EVENT_CLOSE)
  {
    t = SocketEventReason(e);
    printf("Closed: %s\n", sstream_cstr(t));
    SocketClose(s);
    break;
  }
}

*/

#endif
