int nntp_post(const char *msg)
{
  struct NntpData *nntp_data, nntp_tmp;
  char buf[LONG_STRING];

  if (Context && Context->magic == MUTT_NNTP)
    nntp_data = Context->data;
  else
  {
    CurrentNewsSrv = nntp_select_server(NewsServer, false);
    if (!CurrentNewsSrv)
      return -1;

    nntp_data = &nntp_tmp;
    nntp_data->nserv = CurrentNewsSrv;
    nntp_data->group = NULL;
  }

  FILE *fp = mutt_file_fopen(msg, "r");
  if (!fp)
  {
    mutt_perror(msg);
    return -1;
  }

  mutt_str_strfcpy(buf, "POST\r\n", sizeof(buf));
  if (nntp_query(nntp_data, buf, sizeof(buf)) < 0)
  {
    mutt_file_fclose(&fp);
    return -1;
  }
  if (buf[0] != '3')
  {
    mutt_error(_("Can't post article: %s"), buf);
    mutt_file_fclose(&fp);
    return -1;
  }

  buf[0] = '.';
  buf[1] = '\0';
  while (fgets(buf + 1, sizeof(buf) - 2, fp))
  {
    size_t len = strlen(buf);
    if (buf[len - 1] == '\n')
    {
      buf[len - 1] = '\r';
      buf[len] = '\n';
      len++;
      buf[len] = '\0';
    }
    if (mutt_socket_send_d(nntp_data->nserv->conn,
                           buf[1] == '.' ? buf : buf + 1, MUTT_SOCK_LOG_HDR) < 0)
    {
      mutt_file_fclose(&fp);
      return nntp_connect_error(nntp_data->nserv);
    }
  }
  mutt_file_fclose(&fp);

  if ((buf[strlen(buf) - 1] != '\n' &&
       mutt_socket_send_d(nntp_data->nserv->conn, "\r\n", MUTT_SOCK_LOG_HDR) < 0) ||
      mutt_socket_send_d(nntp_data->nserv->conn, ".\r\n", MUTT_SOCK_LOG_HDR) < 0 ||
      mutt_socket_readln(buf, sizeof(buf), nntp_data->nserv->conn) < 0)
  {
    return nntp_connect_error(nntp_data->nserv);
  }
  if (buf[0] != '2')
  {
    mutt_error(_("Can't post article: %s"), buf);
    return -1;
  }
  return 0;
}