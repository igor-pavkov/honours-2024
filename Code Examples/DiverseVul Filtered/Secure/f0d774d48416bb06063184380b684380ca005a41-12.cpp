void mpvio_info(Vio *vio, MYSQL_PLUGIN_VIO_INFO *info)
{
  bzero(info, sizeof(*info));
  switch (vio->type) {
  case VIO_TYPE_TCPIP:
    info->protocol= MYSQL_VIO_TCP;
    info->socket= vio->sd;
    return;
  case VIO_TYPE_SOCKET:
    info->protocol= MYSQL_VIO_SOCKET;
    info->socket= vio->sd;
    return;
  case VIO_TYPE_SSL:
    {
      struct sockaddr addr;
      SOCKET_SIZE_TYPE addrlen= sizeof(addr);
      if (getsockname(vio->sd, &addr, &addrlen))
        return;
      info->protocol= addr.sa_family == AF_UNIX ?
        MYSQL_VIO_SOCKET : MYSQL_VIO_TCP;
      info->socket= vio->sd;
      return;
    }
#ifdef _WIN32
  case VIO_TYPE_NAMEDPIPE:
    info->protocol= MYSQL_VIO_PIPE;
    info->handle= vio->hPipe;
    return;
  case VIO_TYPE_SHARED_MEMORY:
    info->protocol= MYSQL_VIO_MEMORY;
#ifdef HAVE_SMEM
    info->handle= vio->handle_file_map; /* or what ? */
#endif
    return;
#endif
  default: DBUG_ASSERT(0);
  }
}