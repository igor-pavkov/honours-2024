static int wait_for_data(my_socket fd, uint timeout)
{
#ifdef HAVE_POLL
  struct pollfd ufds;
  int res;
  DBUG_ENTER("wait_for_data");

  DBUG_PRINT("info", ("polling"));
  ufds.fd= fd;
  ufds.events= POLLIN | POLLPRI;
  if (!(res= poll(&ufds, 1, (int) timeout*1000)))
  {
    DBUG_PRINT("info", ("poll timed out"));
    errno= EINTR;
    DBUG_RETURN(-1);
  }
  DBUG_PRINT("info",
             ("poll result: %d  errno: %d  revents: 0x%02d  events: 0x%02d",
              res, errno, ufds.revents, ufds.events));
  if (res < 0 || !(ufds.revents & (POLLIN | POLLPRI)))
    DBUG_RETURN(-1);
  /*
    At this point, we know that something happened on the socket.
    But this does not means that everything is alright.
    The connect might have failed. We need to retrieve the error code
    from the socket layer. We must return success only if we are sure
    that it was really a success. Otherwise we might prevent the caller
    from trying another address to connect to.
  */
  {
    int         s_err;
    socklen_t   s_len= sizeof(s_err);

    DBUG_PRINT("info", ("Get SO_ERROR from non-blocked connected socket."));
    res= getsockopt(fd, SOL_SOCKET, SO_ERROR, &s_err, &s_len);
    DBUG_PRINT("info", ("getsockopt res: %d  s_err: %d", res, s_err));
    if (res)
      DBUG_RETURN(res);
    /* getsockopt() was successful, check the retrieved status value. */
    if (s_err)
    {
      errno= s_err;
      DBUG_RETURN(-1);
    }
    /* Status from connect() is zero. Socket is successfully connected. */
  }
  DBUG_RETURN(0);
#else
  SOCKOPT_OPTLEN_TYPE s_err_size = sizeof(uint);
  fd_set sfds;
  struct timeval tv;
  time_t start_time, now_time;
  int res, s_err;
  DBUG_ENTER("wait_for_data");

  if (fd >= FD_SETSIZE)				/* Check if wrong error */
    DBUG_RETURN(0);					/* Can't use timeout */

  /*
    Our connection is "in progress."  We can use the select() call to wait
    up to a specified period of time for the connection to suceed.
    If select() returns 0 (after waiting howevermany seconds), our socket
    never became writable (host is probably unreachable.)  Otherwise, if
    select() returns 1, then one of two conditions exist:
   
    1. An error occured.  We use getsockopt() to check for this.
    2. The connection was set up sucessfully: getsockopt() will
    return 0 as an error.
   
    Thanks goes to Andrew Gierth <andrew@erlenstar.demon.co.uk>
    who posted this method of timing out a connect() in
    comp.unix.programmer on August 15th, 1997.
  */

  FD_ZERO(&sfds);
  FD_SET(fd, &sfds);
  /*
    select could be interrupted by a signal, and if it is, 
    the timeout should be adjusted and the select restarted
    to work around OSes that don't restart select and 
    implementations of select that don't adjust tv upon
    failure to reflect the time remaining
   */
  start_time= my_time(0);
  for (;;)
  {
    tv.tv_sec = (long) timeout;
    tv.tv_usec = 0;
#if defined(HPUX10)
    if ((res = select(fd+1, NULL, (int*) &sfds, NULL, &tv)) > 0)
      break;
#else
    if ((res = select(fd+1, NULL, &sfds, NULL, &tv)) > 0)
      break;
#endif
    if (res == 0)					/* timeout */
      DBUG_RETURN(-1);
    now_time= my_time(0);
    timeout-= (uint) (now_time - start_time);
    if (errno != EINTR || (int) timeout <= 0)
      DBUG_RETURN(-1);
  }

  /*
    select() returned something more interesting than zero, let's
    see if we have any errors.  If the next two statements pass,
    we've got an open socket!
  */

  s_err=0;
  if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*) &s_err, &s_err_size) != 0)
    DBUG_RETURN(-1);

  if (s_err)
  {						/* getsockopt could succeed */
    errno = s_err;
    DBUG_RETURN(-1);					/* but return an error... */
  }
  DBUG_RETURN(0);					/* ok */
#endif /* HAVE_POLL */
}