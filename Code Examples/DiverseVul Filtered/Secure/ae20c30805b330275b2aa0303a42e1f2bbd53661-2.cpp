static int makeNonBlocking(int sockfd)
{
#ifdef   WIN32
  ULONG  ioctl_opt = 1;
  return ioctlsocket(sockfd, FIONBIO, &ioctl_opt);
#else
  int    result  = fcntl(sockfd, F_SETFL, O_NONBLOCK);
  return result >= 0 ? result : SOCKET_ERROR ;
#endif
}