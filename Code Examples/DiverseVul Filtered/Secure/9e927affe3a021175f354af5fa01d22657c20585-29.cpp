int nntp_open_connection(struct NntpServer *nserv)
{
  struct Connection *conn = nserv->conn;
  char buf[STRING];
  int cap;
  bool posting = false, auth = true;

  if (nserv->status == NNTP_OK)
    return 0;
  if (nserv->status == NNTP_BYE)
    return -1;
  nserv->status = NNTP_NONE;

  if (mutt_socket_open(conn) < 0)
    return -1;

  if (mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    return nntp_connect_error(nserv);

  if (mutt_str_strncmp("200", buf, 3) == 0)
    posting = true;
  else if (mutt_str_strncmp("201", buf, 3) != 0)
  {
    mutt_socket_close(conn);
    mutt_str_remove_trailing_ws(buf);
    mutt_error("%s", buf);
    return -1;
  }

  /* get initial capabilities */
  cap = nntp_capabilities(nserv);
  if (cap < 0)
    return -1;

  /* tell news server to switch to mode reader if it isn't so */
  if (cap > 0)
  {
    if (mutt_socket_send(conn, "MODE READER\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }

    if (mutt_str_strncmp("200", buf, 3) == 0)
      posting = true;
    else if (mutt_str_strncmp("201", buf, 3) == 0)
      posting = false;
    /* error if has capabilities, ignore result if no capabilities */
    else if (nserv->hasCAPABILITIES)
    {
      mutt_socket_close(conn);
      mutt_error(_("Could not switch to reader mode."));
      return -1;
    }

    /* recheck capabilities after MODE READER */
    if (nserv->hasCAPABILITIES)
    {
      cap = nntp_capabilities(nserv);
      if (cap < 0)
        return -1;
    }
  }

  mutt_message(_("Connected to %s. %s"), conn->account.host,
               posting ? _("Posting is ok.") : _("Posting is NOT ok."));
  mutt_sleep(1);

#ifdef USE_SSL
  /* Attempt STARTTLS if available and desired. */
  if (nserv->use_tls != 1 && (nserv->hasSTARTTLS || SslForceTls))
  {
    if (nserv->use_tls == 0)
    {
      nserv->use_tls =
          SslForceTls || query_quadoption(SslStarttls,
                                          _("Secure connection with TLS?")) == MUTT_YES ?
              2 :
              1;
    }
    if (nserv->use_tls == 2)
    {
      if (mutt_socket_send(conn, "STARTTLS\r\n") < 0 ||
          mutt_socket_readln(buf, sizeof(buf), conn) < 0)
      {
        return nntp_connect_error(nserv);
      }
      if (mutt_str_strncmp("382", buf, 3) != 0)
      {
        nserv->use_tls = 0;
        mutt_error("STARTTLS: %s", buf);
      }
      else if (mutt_ssl_starttls(conn))
      {
        nserv->use_tls = 0;
        nserv->status = NNTP_NONE;
        mutt_socket_close(nserv->conn);
        mutt_error(_("Could not negotiate TLS connection"));
        return -1;
      }
      else
      {
        /* recheck capabilities after STARTTLS */
        cap = nntp_capabilities(nserv);
        if (cap < 0)
          return -1;
      }
    }
  }
#endif

  /* authentication required? */
  if (conn->account.flags & MUTT_ACCT_USER)
  {
    if (!conn->account.user[0])
      auth = false;
  }
  else
  {
    if (mutt_socket_send(conn, "STAT\r\n") < 0 ||
        mutt_socket_readln(buf, sizeof(buf), conn) < 0)
    {
      return nntp_connect_error(nserv);
    }
    if (mutt_str_strncmp("480", buf, 3) != 0)
      auth = false;
  }

  /* authenticate */
  if (auth && nntp_auth(nserv) < 0)
    return -1;

  /* get final capabilities after authentication */
  if (nserv->hasCAPABILITIES && (auth || cap > 0))
  {
    cap = nntp_capabilities(nserv);
    if (cap < 0)
      return -1;
    if (cap > 0)
    {
      mutt_socket_close(conn);
      mutt_error(_("Could not switch to reader mode."));
      return -1;
    }
  }

  /* attempt features */
  if (nntp_attempt_features(nserv) < 0)
    return -1;

  nserv->status = NNTP_OK;
  return 0;
}