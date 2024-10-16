int nntp_check_new_groups(struct NntpServer *nserv)
{
  struct NntpData nntp_data;
  time_t now;
  struct tm *tm = NULL;
  char buf[LONG_STRING];
  char *msg = _("Checking for new newsgroups...");
  unsigned int i;
  int rc, update_active = false;

  if (!nserv || !nserv->newgroups_time)
    return -1;

  /* check subscribed newsgroups for new articles */
  if (ShowNewNews)
  {
    mutt_message(_("Checking for new messages..."));
    for (i = 0; i < nserv->groups_num; i++)
    {
      struct NntpData *data = nserv->groups_list[i];

      if (data && data->subscribed)
      {
        rc = nntp_group_poll(data, 1);
        if (rc < 0)
          return -1;
        if (rc > 0)
          update_active = true;
      }
    }
    /* select current newsgroup */
    if (Context && Context->magic == MUTT_NNTP)
    {
      buf[0] = '\0';
      if (nntp_query((struct NntpData *) Context->data, buf, sizeof(buf)) < 0)
        return -1;
    }
  }
  else if (nserv->newgroups_time)
    return 0;

  /* get list of new groups */
  mutt_message(msg);
  if (nntp_date(nserv, &now) < 0)
    return -1;
  nntp_data.nserv = nserv;
  if (Context && Context->magic == MUTT_NNTP)
    nntp_data.group = ((struct NntpData *) Context->data)->group;
  else
    nntp_data.group = NULL;
  i = nserv->groups_num;
  tm = gmtime(&nserv->newgroups_time);
  snprintf(buf, sizeof(buf), "NEWGROUPS %02d%02d%02d %02d%02d%02d GMT\r\n",
           tm->tm_year % 100, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
           tm->tm_min, tm->tm_sec);
  rc = nntp_fetch_lines(&nntp_data, buf, sizeof(buf), msg, nntp_add_group, nserv);
  if (rc)
  {
    if (rc > 0)
    {
      mutt_error("NEWGROUPS: %s", buf);
    }
    return -1;
  }

  /* new groups found */
  rc = 0;
  if (nserv->groups_num != i)
  {
    int groups_num = i;

    nserv->newgroups_time = now;
    for (; i < nserv->groups_num; i++)
    {
      struct NntpData *data = nserv->groups_list[i];
      data->new = true;
    }

    /* loading descriptions */
    if (NntpLoadDescription)
    {
      unsigned int count = 0;
      struct Progress progress;

      mutt_progress_init(&progress, _("Loading descriptions..."),
                         MUTT_PROGRESS_MSG, ReadInc, nserv->groups_num - i);
      for (i = groups_num; i < nserv->groups_num; i++)
      {
        struct NntpData *data = nserv->groups_list[i];

        if (get_description(data, NULL, NULL) < 0)
          return -1;
        mutt_progress_update(&progress, ++count, -1);
      }
    }
    update_active = true;
    rc = 1;
  }
  if (update_active)
    nntp_active_save_cache(nserv);
  mutt_clear_error();
  return rc;
}