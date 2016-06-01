#include <stentext.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <unistd.h>

struct SocketDescriptor
{
  int fd;
};

REFDEF(SocketDescriptor);

struct Socket
{
  ARRAY(SocketDescriptor) connectingFds;
  REF(SocketDescriptor) fd;
};

REFDEF(Socket);

static void _SocketDescriptorFinalize(REF(SocketDescriptor) ctx)
{
  if(GET(ctx)->fd != -1)
  {
    close(GET(ctx)->fd);
  }
}

void SocketClose(REF(Socket) ctx)
{
  size_t i = 0;

  if(TRYGET(GET(ctx)->fd) != NULL)
  {
    FREE(GET(ctx)->fd);
  }

  if(TRYGET(GET(ctx)->connectingFds) != NULL)
  {
    for(i = 0; i < ARRAY_SIZE(GET(ctx)->connectingFds); i++)
    {
      FREE(ARRAY_AT(GET(ctx)->connectingFds, i));
    }

    ARRAY_CLEAR(GET(ctx)->connectingFds);
  }
}

static void _SocketFinalize(REF(Socket) ctx)
{
  printf("~Socket\n");
  SocketClose(ctx);

  if(TRYGET(GET(ctx)->connectingFds) != NULL)
  {
    FREE(GET(ctx)->connectingFds);
  }
}

REF(Socket) SocketCreate()
{
  REF(Socket) rtn = {};

  rtn = CALLOC(Socket);
  FINALIZER(rtn, _SocketFinalize);
  GET(rtn)->connectingFds = ARRAY_ALLOC(SocketDescriptor);

  return rtn;
}

void SocketConnect(REF(Socket) ctx, char *host, int port)
{
  struct addrinfo hints = {};
  struct addrinfo *result = NULL;
  struct addrinfo *rp = NULL;
  int s = 0;
  REF(String) portStr = {};
  int flags = 0;
  int fd = -1;
  REF(SocketDescriptor) sock = {};

  SocketClose(ctx);

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  portStr = StringEmpty();
  StringAddInt(portStr, port);
  s = getaddrinfo(host, StringCStr(portStr), &hints, &result);
  FREE(portStr);

  if(s != 0)
  {
    THROW(0, "Failed to find address");
  }

  for(rp = result; rp != NULL; rp = rp->ai_next)
  {
    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if(fd == -1)
    {
      continue;
    }

    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if(connect(fd, rp->ai_addr, rp->ai_addrlen) != -1)
    {
      sock = CALLOC(SocketDescriptor);
      GET(sock)->fd = fd;
      FINALIZER(sock, _SocketDescriptorFinalize);
      GET(ctx)->fd = sock;
      break;
    }
    else
    {
      if(errno == EINPROGRESS) 
      {
        sock = CALLOC(SocketDescriptor);
        GET(sock)->fd = fd;
        FINALIZER(sock, _SocketDescriptorFinalize);
        ARRAY_ADD(GET(ctx)->connectingFds, sock);
      }
      else
      {
        close(fd);
        fd = -1;
        continue;
      }
    }
  }

  freeaddrinfo(result);
}

int SocketConnected(REF(Socket) ctx)
{
  size_t i = 0;
  REF(SocketDescriptor) sock = {};
  int result = 0;
  socklen_t result_len = sizeof(result);
  fd_set myset = {};
  struct timeval tv = {};

  if(TRYGET(GET(ctx)->fd) != NULL)
  {
    return 1;
  }

  for(i = 0; i < ARRAY_SIZE(GET(ctx)->connectingFds); i++)
  {
    sock = ARRAY_AT(GET(ctx)->connectingFds, i);

    tv.tv_sec = 0; 
    tv.tv_usec = 0; 
    FD_ZERO(&myset); 
    FD_SET(GET(sock)->fd, &myset); 
    result = select(GET(sock)->fd + 1, NULL, &myset, NULL, &tv); 

    if(result < 1)
    { 
      continue;
    }

    if(getsockopt(GET(sock)->fd, SOL_SOCKET, SO_ERROR,
      &result, &result_len) < 0 || result != 0)
    {
      ARRAY_REMOVEAT(GET(ctx)->connectingFds, i);
      i--;
      FREE(sock);

      if(ARRAY_SIZE(GET(ctx)->connectingFds) <= 0)
      {
        THROW(0, "Failed to connect");
      }

      continue;
    }

    ARRAY_REMOVEAT(GET(ctx)->connectingFds, i);
    i--;
    GET(ctx)->fd = sock;

    for(i = 0; i < ARRAY_SIZE(GET(ctx)->connectingFds); i++)
    {
      FREE(ARRAY_AT(GET(ctx)->connectingFds, i));
    }

    ARRAY_CLEAR(GET(ctx)->connectingFds);

    return 1;
  }

  return 0;
}

void safe_main(REF(Object) userData)
{
  REF(Socket) socket = {};
  size_t i = 0;

  FREE(userData);
  socket = SocketCreate();
  SocketConnect(socket, "www.google.com", 80);

  for(i = 0; i < 3; i++)
  {
    if(SocketConnected(socket) != 0)
    {
      printf("Connected!\n");
      break;
    }

    printf("Connecting...\n");
    sleep(1);
  }

  FREE(socket);
}

int main(int argc, char *argv[])
{
  REF(Object) userData = {};
  REF(Exception) exception = {};
  int rtn = 0;

  userData = CALLOC(Object);

  exception = TRY(safe_main, userData);

  if(TRYGET(exception) != NULL)
  {
    printf("Exception: %s %s %i\n", GET(exception)->message,
      GET(exception)->file, GET(exception)->line);

    FREE(exception);
    rtn = 1;
  }

  StentStats();
  StentCleanup();

  return rtn;
}
