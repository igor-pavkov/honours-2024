static int nntp_capabilities(struct NntpServer *nserv)
{
  struct Connection *conn = nserv->conn;
  bool mode_reader = false;
  char buf[LONG_STRING];
  char authinfo[LONG_STRING] = "";

  nserv->hasCAPABILITIES = false;
  nserv->hasSTARTTLS = false;
  nserv->hasDATE = false;
  nserv->hasLIST_NEWSGROUPS = false;
  nserv->hasLISTGROUP = false;
  nserv->hasLISTGROUPrange = false;
  nserv->hasOVER = false;
  FREE(&nserv->authenticators);

  if (mutt_socket_send(conn, "CAPABILITIES\r\n") < 0 ||
      mutt_socket_readln(buf, sizeof(buf), conn) < 0)
  {
    return nntp_connect_error(nserv);
  }

  /* no capabilities */
  if (mutt_str_strncmp("101", buf, 3) != 0)
    return 1;
  nserv->hasCAPABILITIES = true;

  /* parse capabilities */
  do
  {
    if (mutt_socket_readln(buf, sizeof(buf), conn) < 0)
      return nntp_connect_error(nserv);
    if (mutt_str_strcmp("STARTTLS", buf) == 0)
      nserv->hasSTARTTLS = true;
    else if (mutt_str_strcmp("MODE-READER", buf) == 0)
      mode_reader = true;
    else if (mutt_str_strcmp("READER", buf) == 0)
    {
      nserv->hasDATE = true;
      nserv->hasLISTGROUP = true;
      nserv->hasLISTGROUPrange = true;
    }
    else if (mutt_str_strncmp("AUTHINFO ", buf, 9) == 0)
    {
      mutt_str_strcat(buf, sizeof(buf), " ");
      mutt_str_strfcpy(authinfo, buf + 8, sizeof(authinfo));
    }
#ifdef USE_SASL
    else if (mutt_str_strncmp("SASL ", buf, 5) == 0)
    {
      char *p = buf + 5;
      while (*p == ' ')
        p++;
      nserv->authenticators = mutt_str_strdup(p);
    }
#endif
    else if (mutt_str_strcmp("OVER", buf) == 0)
      nserv->hasOVER = true;
    else if (mutt_str_strncmp("LIST ", buf, 5) == 0)
    {
      char *p = strstr(buf, " NEWSGROUPS");
      if (p)
      {
        p += 11;
        if (*p == '\0' || *p == ' ')
          nserv->hasLIST_NEWSGROUPS = true;
      }
    }
  } while (mutt_str_strcmp(".", buf) != 0);
  *buf = '\0';
#ifdef USE_SASL
  if (nserv->authenticators && strcasestr(authinfo, " SASL "))
    mutt_str_strfcpy(buf, nserv->authenticators, sizeof(buf));
#endif
  if (strcasestr(authinfo, " USER "))
  {
    if (*buf)
      mutt_str_strcat(buf, sizeof(buf), " ");
    mutt_str_strcat(buf, sizeof(buf), "USER");
  }
  mutt_str_replace(&nserv->authenticators, buf);

  /* current mode is reader */
  if (nserv->hasDATE)
    return 0;

  /* server is mode-switching, need to switch to reader mode */
  if (mode_reader)
    return 1;

  mutt_socket_close(conn);
  nserv->status = NNTP_BYE;
  mutt_error(_("Server doesn't support reader mode."));
  return -1;
}