static int nntp_attempt_features(struct NntpServer *nserv)
{
  struct Connection *conn = nserv->conn;
  char buf[LONG_STRING];

  /* no CAPABILITIES, trying DATE, LISTGROUP, LIST NEWSGROUPS */
  if (!nserv->hasCAPABILITIES)
  {
    if (mutt_socket_send(conn, "DATE\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("500", buf, 3) != 0)
      nserv->hasDATE = true;

    if (mutt_socket_send(conn, "LISTGROUP\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("500", buf, 3) != 0)
      nserv->hasLISTGROUP = true;

    if (mutt_socket_send(conn, "LIST NEWSGROUPS +\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("500", buf, 3) != 0)
      nserv->hasLIST_NEWSGROUPS = true;
    if (mutt_str_strncmp("215", buf, 3) == 0)
    {
      do
      {
        if (mutt_socket_readln(buf, sizeof(buf), conn) < 0)
          return nntp_connect_error(nserv);
      } while (mutt_str_strcmp(".", buf) != 0);
    }
  }

  /* no LIST NEWSGROUPS, trying XGTITLE */
  if (!nserv->hasLIST_NEWSGROUPS)
  {
    if (mutt_socket_send(conn, "XGTITLE\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("500", buf, 3) != 0)
      nserv->hasXGTITLE = true;
  }

  /* no OVER, trying XOVER */
  if (!nserv->hasOVER)
  {
    if (mutt_socket_send(conn, "XOVER\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("500", buf, 3) != 0)
      nserv->hasXOVER = true;
  }

  /* trying LIST OVERVIEW.FMT */
  if (nserv->hasOVER || nserv->hasXOVER)
  {
    if (mutt_socket_send(conn, "LIST OVERVIEW.FMT\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("215", buf, 3) != 0)
      nserv->overview_fmt = OverviewFmt;
    else
    {
      int cont = 0;
      size_t buflen = 2 * LONG_STRING, off = 0, b = 0;

      if (nserv->overview_fmt)
        FREE(&nserv->overview_fmt);
      nserv->overview_fmt = mutt_mem_malloc(buflen);

      while (true)
      {
        if (buflen - off < LONG_STRING)
        {
          buflen *= 2;
          mutt_mem_realloc(&nserv->overview_fmt, buflen);
        }

        const int chunk =
            mutt_socket_readln(nserv->overview_fmt + off, buflen - off, conn);
        if (chunk < 0)
        {
          FREE(&nserv->overview_fmt);
          return nntp_connect_error(nserv);
        }

        if (!cont && (mutt_str_strcmp(".", nserv->overview_fmt + off) == 0))
          break;

        cont = chunk >= buflen - off ? 1 : 0;
        off += strlen(nserv->overview_fmt + off);
        if (!cont)
        {
          char *colon = NULL;

          if (nserv->overview_fmt[b] == ':')
          {
            memmove(nserv->overview_fmt + b, nserv->overview_fmt + b + 1, off - b - 1);
            nserv->overview_fmt[off - 1] = ':';
          }
          colon = strchr(nserv->overview_fmt + b, ':');
          if (!colon)
            nserv->overview_fmt[off++] = ':';
          else if (strcmp(colon + 1, "full") != 0)
            off = colon + 1 - nserv->overview_fmt;
          if (strcasecmp(nserv->overview_fmt + b, "Bytes:") == 0)
          {
            size_t len = strlen(nserv->overview_fmt + b);
            mutt_str_strfcpy(nserv->overview_fmt + b, "Content-Length:", len + 1);
            off = b + len;
          }
          nserv->overview_fmt[off++] = '\0';
          b = off;
        }
      }
      nserv->overview_fmt[off++] = '\0';
      mutt_mem_realloc(&nserv->overview_fmt, off);
    }
  }
  return 0;
}