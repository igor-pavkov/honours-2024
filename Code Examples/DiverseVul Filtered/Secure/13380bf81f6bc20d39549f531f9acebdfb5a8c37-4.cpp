int my_connect(my_socket fd, const struct sockaddr *name, uint namelen,
	       uint timeout)
{
#if defined(__WIN__)
  DBUG_ENTER("my_connect");
  DBUG_RETURN(connect(fd, (struct sockaddr*) name, namelen));
#else
  int flags, res, s_err;
  DBUG_ENTER("my_connect");
  DBUG_PRINT("enter", ("fd: %d  timeout: %u", fd, timeout));

  /*
    If they passed us a timeout of zero, we should behave
    exactly like the normal connect() call does.
  */

  if (timeout == 0)
    DBUG_RETURN(connect(fd, (struct sockaddr*) name, namelen));

  flags = fcntl(fd, F_GETFL, 0);	  /* Set socket to not block */
#ifdef O_NONBLOCK
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);  /* and save the flags..  */
#endif

  DBUG_PRINT("info", ("connecting non-blocking"));
  res= connect(fd, (struct sockaddr*) name, namelen);
  DBUG_PRINT("info", ("connect result: %d  errno: %d", res, errno));
  s_err= errno;			/* Save the error... */
  fcntl(fd, F_SETFL, flags);
  if ((res != 0) && (s_err != EINPROGRESS))
  {
    errno= s_err;			/* Restore it */
    DBUG_RETURN(-1);
  }
  if (res == 0)				/* Connected quickly! */
    DBUG_RETURN(0);
  DBUG_RETURN(wait_for_data(fd, timeout));
#endif
}