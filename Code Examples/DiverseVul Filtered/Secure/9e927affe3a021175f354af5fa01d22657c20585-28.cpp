int nntp_active_fetch(struct NntpServer *nserv, bool new)
{
  struct NntpData nntp_data;
  char msg[STRING];
  char buf[LONG_STRING];
  unsigned int i;
  int rc;

  snprintf(msg, sizeof(msg), _("Loading list of groups from server %s..."),
           nserv->conn->account.host);
  mutt_message(msg);
  if (nntp_date(nserv, &nserv->newgroups_time) < 0)
    return -1;

  nntp_data.nserv = nserv;
  nntp_data.group = NULL;
  i = nserv->groups_num;
  mutt_str_strfcpy(buf, "LIST\r\n", sizeof(buf));
  rc = nntp_fetch_lines(&nntp_data, buf, sizeof(buf), msg, nntp_add_group, nserv);
  if (rc)
  {
    if (rc > 0)
    {
      mutt_error("LIST: %s", buf);
    }
    return -1;
  }

  if (new)
  {
    for (; i < nserv->groups_num; i++)
    {
      struct NntpData *data = nserv->groups_list[i];
      data->new = true;
    }
  }

  for (i = 0; i < nserv->groups_num; i++)
  {
    struct NntpData *data = nserv->groups_list[i];

    if (data && data->deleted && !data->newsrc_ent)
    {
      nntp_delete_group_cache(data);
      mutt_hash_delete(nserv->groups_hash, data->group, NULL);
      nserv->groups_list[i] = NULL;
    }
  }

  if (NntpLoadDescription)
    rc = get_description(&nntp_data, "*", _("Loading descriptions..."));

  nntp_active_save_cache(nserv);
  if (rc < 0)
    return -1;
  mutt_clear_error();
  return 0;
}