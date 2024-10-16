static int nntp_fetch_lines(struct NntpData *nntp_data, char *query, size_t qlen,
                            const char *msg, int (*funct)(char *, void *), void *data)
{
  int done = false;
  int rc;

  while (!done)
  {
    char buf[LONG_STRING];
    char *line = NULL;
    unsigned int lines = 0;
    size_t off = 0;
    struct Progress progress;

    if (msg)
      mutt_progress_init(&progress, msg, MUTT_PROGRESS_MSG, ReadInc, 0);

    mutt_str_strfcpy(buf, query, sizeof(buf));
    if (nntp_query(nntp_data, buf, sizeof(buf)) < 0)
      return -1;
    if (buf[0] != '2')
    {
      mutt_str_strfcpy(query, buf, qlen);
      return 1;
    }

    line = mutt_mem_malloc(sizeof(buf));
    rc = 0;

    while (true)
    {
      char *p = NULL;
      int chunk = mutt_socket_readln_d(buf, sizeof(buf), nntp_data->nserv->conn,
                                       MUTT_SOCK_LOG_HDR);
      if (chunk < 0)
      {
        nntp_data->nserv->status = NNTP_NONE;
        break;
      }

      p = buf;
      if (!off && buf[0] == '.')
      {
        if (buf[1] == '\0')
        {
          done = true;
          break;
        }
        if (buf[1] == '.')
          p++;
      }

      mutt_str_strfcpy(line + off, p, sizeof(buf));

      if (chunk >= sizeof(buf))
        off += strlen(p);
      else
      {
        if (msg)
          mutt_progress_update(&progress, ++lines, -1);

        if (rc == 0 && funct(line, data) < 0)
          rc = -2;
        off = 0;
      }

      mutt_mem_realloc(&line, off + sizeof(buf));
    }
    FREE(&line);
    funct(NULL, data);
  }
  return rc;
}