static int nntp_auth(struct NntpServer *nserv)
{
  struct Connection *conn = nserv->conn;
  char buf[LONG_STRING];
  char authenticators[LONG_STRING] = "USER";
  char *method = NULL, *a = NULL, *p = NULL;
  unsigned char flags = conn->account.flags;

  while (true)
  {
    /* get login and password */
    if ((mutt_account_getuser(&conn->account) < 0) || (conn->account.user[0] == '\0') ||
        (mutt_account_getpass(&conn->account) < 0) || (conn->account.pass[0] == '\0'))
    {
      break;
    }

    /* get list of authenticators */
    if (NntpAuthenticators && *NntpAuthenticators)
      mutt_str_strfcpy(authenticators, NntpAuthenticators, sizeof(authenticators));
    else if (nserv->hasCAPABILITIES)
    {
      mutt_str_strfcpy(authenticators, NONULL(nserv->authenticators), sizeof(authenticators));
      p = authenticators;
      while (*p)
      {
        if (*p == ' ')
          *p = ':';
        p++;
      }
    }
    p = authenticators;
    while (*p)
    {
      *p = toupper(*p);
      p++;
    }

    mutt_debug(1, "available methods: %s\n", nserv->authenticators);
    a = authenticators;
    while (true)
    {
      if (!a)
      {
        mutt_error(_("No authenticators available"));
        break;
      }

      method = a;
      a = strchr(a, ':');
      if (a)
        *a++ = '\0';

      /* check authenticator */
      if (nserv->hasCAPABILITIES)
      {
        char *m = NULL;

        if (!nserv->authenticators)
          continue;
        m = strcasestr(nserv->authenticators, method);
        if (!m)
          continue;
        if (m > nserv->authenticators && *(m - 1) != ' ')
          continue;
        m += strlen(method);
        if (*m != '\0' && *m != ' ')
          continue;
      }
      mutt_debug(1, "trying method %s\n", method);

      /* AUTHINFO USER authentication */
      if (strcmp(method, "USER") == 0)
      {
        mutt_message(_("Authenticating (%s)..."), method);
        snprintf(buf, sizeof(buf), "AUTHINFO USER %s\r\n", conn->account.user);
        if (mutt_socket_send(conn, buf) < 0 ||
            mutt_socket_readln(buf, sizeof(buf), conn) < 0)
        {
          break;
        }

        /* authenticated, password is not required */
        if (mutt_str_strncmp("281", buf, 3) == 0)
          return 0;

        /* username accepted, sending password */
        if (mutt_str_strncmp("381", buf, 3) == 0)
        {
          if (DebugLevel < MUTT_SOCK_LOG_FULL)
            mutt_debug(MUTT_SOCK_LOG_CMD, "%d> AUTHINFO PASS *\n", conn->fd);
          snprintf(buf, sizeof(buf), "AUTHINFO PASS %s\r\n", conn->account.pass);
          if (mutt_socket_send_d(conn, buf, MUTT_SOCK_LOG_FULL) < 0 ||
              mutt_socket_readln(buf, sizeof(buf), conn) < 0)
          {
            break;
          }

          /* authenticated */
          if (mutt_str_strncmp("281", buf, 3) == 0)
            return 0;
        }

        /* server doesn't support AUTHINFO USER, trying next method */
        if (*buf == '5')
          continue;
      }

      else
      {
#ifdef USE_SASL
        sasl_conn_t *saslconn = NULL;
        sasl_interact_t *interaction = NULL;
        int rc;
        char inbuf[LONG_STRING] = "";
        const char *mech = NULL;
        const char *client_out = NULL;
        unsigned int client_len, len;

        if (mutt_sasl_client_new(conn, &saslconn) < 0)
        {
          mutt_debug(1, "error allocating SASL connection.\n");
          continue;
        }

        while (true)
        {
          rc = sasl_client_start(saslconn, method, &interaction, &client_out,
                                 &client_len, &mech);
          if (rc != SASL_INTERACT)
            break;
          mutt_sasl_interact(interaction);
        }
        if (rc != SASL_OK && rc != SASL_CONTINUE)
        {
          sasl_dispose(&saslconn);
          mutt_debug(1, "error starting SASL authentication exchange.\n");
          continue;
        }

        mutt_message(_("Authenticating (%s)..."), method);
        snprintf(buf, sizeof(buf), "AUTHINFO SASL %s", method);

        /* looping protocol */
        while (rc == SASL_CONTINUE || (rc == SASL_OK && client_len))
        {
          /* send out client response */
          if (client_len)
          {
            if (DebugLevel >= MUTT_SOCK_LOG_FULL)
            {
              char tmp[LONG_STRING];
              memcpy(tmp, client_out, client_len);
              for (p = tmp; p < tmp + client_len; p++)
              {
                if (*p == '\0')
                  *p = '.';
              }
              *p = '\0';
              mutt_debug(1, "SASL> %s\n", tmp);
            }

            if (*buf)
              mutt_str_strcat(buf, sizeof(buf), " ");
            len = strlen(buf);
            if (sasl_encode64(client_out, client_len, buf + len,
                              sizeof(buf) - len, &len) != SASL_OK)
            {
              mutt_debug(1, "error base64-encoding client response.\n");
              break;
            }
          }

          mutt_str_strcat(buf, sizeof(buf), "\r\n");
          if (DebugLevel < MUTT_SOCK_LOG_FULL)
          {
            if (strchr(buf, ' '))
            {
              mutt_debug(MUTT_SOCK_LOG_CMD, "%d> AUTHINFO SASL %s%s\n",
                         conn->fd, method, client_len ? " sasl_data" : "");
            }
            else
              mutt_debug(MUTT_SOCK_LOG_CMD, "%d> sasl_data\n", conn->fd);
          }
          client_len = 0;
          if (mutt_socket_send_d(conn, buf, MUTT_SOCK_LOG_FULL) < 0 ||
              mutt_socket_readln_d(inbuf, sizeof(inbuf), conn, MUTT_SOCK_LOG_FULL) < 0)
          {
            break;
          }
          if ((mutt_str_strncmp(inbuf, "283 ", 4) != 0) &&
              (mutt_str_strncmp(inbuf, "383 ", 4) != 0))
          {
            if (DebugLevel < MUTT_SOCK_LOG_FULL)
              mutt_debug(MUTT_SOCK_LOG_CMD, "%d< %s\n", conn->fd, inbuf);
            break;
          }
          if (DebugLevel < MUTT_SOCK_LOG_FULL)
          {
            inbuf[3] = '\0';
            mutt_debug(MUTT_SOCK_LOG_CMD, "%d< %s sasl_data\n", conn->fd, inbuf);
          }

          if (strcmp("=", inbuf + 4) == 0)
            len = 0;
          else if (sasl_decode64(inbuf + 4, strlen(inbuf + 4), buf,
                                 sizeof(buf) - 1, &len) != SASL_OK)
          {
            mutt_debug(1, "error base64-decoding server response.\n");
            break;
          }
          else if (DebugLevel >= MUTT_SOCK_LOG_FULL)
          {
            char tmp[LONG_STRING];
            memcpy(tmp, buf, len);
            for (p = tmp; p < tmp + len; p++)
            {
              if (*p == '\0')
                *p = '.';
            }
            *p = '\0';
            mutt_debug(1, "SASL< %s\n", tmp);
          }

          while (true)
          {
            rc = sasl_client_step(saslconn, buf, len, &interaction, &client_out, &client_len);
            if (rc != SASL_INTERACT)
              break;
            mutt_sasl_interact(interaction);
          }
          if (*inbuf != '3')
            break;

          *buf = '\0';
        } /* looping protocol */

        if (rc == SASL_OK && client_len == 0 && *inbuf == '2')
        {
          mutt_sasl_setup_conn(conn, saslconn);
          return 0;
        }

        /* terminate SASL session */
        sasl_dispose(&saslconn);
        if (conn->fd < 0)
          break;
        if (mutt_str_strncmp(inbuf, "383 ", 4) == 0)
        {
          if (mutt_socket_send(conn, "*\r\n") < 0 ||
              mutt_socket_readln(inbuf, sizeof(inbuf), conn) < 0)
          {
            break;
          }
        }

        /* server doesn't support AUTHINFO SASL, trying next method */
        if (*inbuf == '5')
          continue;
#else
        continue;
#endif /* USE_SASL */
      }

      mutt_error(_("%s authentication failed."), method);
      break;
    }
    break;
  }

  /* error */
  nserv->status = NNTP_BYE;
  conn->account.flags = flags;
  if (conn->fd < 0)
  {
    mutt_error(_("Server closed connection!"));
  }
  else
    mutt_socket_close(conn);
  return -1;
}