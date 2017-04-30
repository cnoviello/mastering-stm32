#include <_ansi.h>
#include <_syslist.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <limits.h>
#include <signal.h>
#include <ioLibrary_Driver/Ethernet/socket.h>
#include <../system/include/retarget/retarget-tcp.h>
#include <stdint.h>

#ifdef RETARGET_TCP

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifndef RETARGET_PORT
#define RETARGET_PORT 5000
#endif

int8_t gSock = -1;

uint8_t RetargetInit(int8_t sn) {
  gSock = sn;

  /* Disable I/O buffering for STDOUT stream, so that
   * chars are sent out as soon as they are printed. */
  setvbuf(stdout, NULL, _IONBF, 0);

  /* Open 'sn' socket in TCP mode with a port number equal
   * to the value of RETARGET_PORT macro */
  if(socket(sn, Sn_MR_TCP, RETARGET_PORT, 0) == sn) {
    if(listen(sn) == SOCK_OK)
      return 1;
  }
  return 0;
}

int _isatty(int fd) {
  if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    return 1;

  errno = EBADF;
  return 0;
}

int _write(int fd, char* ptr, int len) {
  int sentlen = 0;
  int buflen = len;

  if(getSn_SR(gSock) == SOCK_ESTABLISHED) {
    if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
      while(1) {
        sentlen = send(gSock, (void*) ptr, buflen);
        if (sentlen == buflen)
          return len;
        else if (sentlen > 0 && sentlen < buflen) {
          buflen -= sentlen;
          ptr += (len - buflen);
        }
        else if (sentlen < 0)
          return EIO;
      }
    }
  } else if(getSn_SR(gSock) != SOCK_LISTEN) {
    /* Remote peer close the connection? */
    close(gSock);
    RetargetInit(gSock);
  }

  errno = EBADF;
  return -1;
}

int _close(int fd) {
  if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    return 0;

  errno = EBADF;
  return -1;
}

int _lseek(int fd, int ptr, int dir) {
  (void) fd;
  (void) ptr;
  (void) dir;

  errno = EBADF;
  return -1;
}

int _read(int fd, char* ptr, int len) {
  if(getSn_SR(gSock) == SOCK_ESTABLISHED) {
    if (fd == STDIN_FILENO) {
      while(1) {
        len = recv(gSock, (void*) ptr, len);
        if (len > 0)
          return len;
        else
          return EIO;
      }
    }
  } else if(getSn_SR(gSock) != SOCK_LISTEN) {
    /* Remote peer close the connection? */
    close(gSock);
    RetargetInit(gSock);
  }

  errno = EBADF;
  return -1;
}

int _fstat(int fd, struct stat* st) {
  if (fd >= STDIN_FILENO && fd <= STDERR_FILENO) {
    st->st_mode = S_IFCHR;
    return 0;
  }

  errno = EBADF;
  return 0;
}

#endif //#if !defined(OS_USE_SEMIHOSTING)
