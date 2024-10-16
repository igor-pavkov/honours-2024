static int nntp_query(struct NntpData *nntp_data, char *line, size_t linelen)
{
  struct NntpServer *nserv = nntp_data->nserv;
  char buf[LONG_STRING] = { 0 };

  if (nserv->status == NNTP_BYE)
    return -1;

  while (true)
  {
    if (nserv->status == NNTP_OK)
    {
      int rc = 0;

      if (*line)
        rc = mutt_socket_send(nserv->conn, line);
      else if (nntp_data->group)
      {
        snprintf(buf, sizeof(buf), "GROUP %s\r\n", nntp_data->group);
        rc = mutt_socket_send(nserv->conn, buf);
      }
      if (rc >= 0)
        rc = mutt_socket_readln(buf, sizeof(buf), nserv->conn);
      if (rc >= 0)
        break;
    }

    /* reconnect */
    while (true)
    {
      nserv->status = NNTP_NONE;
      if (nntp_open_connection(nserv) == 0)
        break;

      snprintf(buf, sizeof(buf), _("Connection to %s lost. Reconnect?"),
               nserv->conn->account.host);
      if (mutt_yesorno(buf, MUTT_YES) != MUTT_YES)
      {
        nserv->status = NNTP_BYE;
        return -1;
      }
    }

    /* select newsgroup after reconnection */
    if (nntp_data->group)
    {
      snprintf(buf, sizeof(buf), "GROUP %s\r\n", nntp_data->group);
      if (mutt_socket_send(nserv->conn, buf) < 0 ||
          mutt_socket_readln(buf, sizeof(buf), nserv->conn) < 0)
      {
        return nntp_connect_error(nserv);
      }
    }
    if (!*line)
      break;
  }

  mutt_str_strfcpy(line, buf, linelen);
  return 0;
}