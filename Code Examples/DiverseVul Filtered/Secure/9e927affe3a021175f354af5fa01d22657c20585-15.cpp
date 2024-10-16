static int nntp_group_poll(struct NntpData *nntp_data, int update_stat)
{
  char buf[LONG_STRING] = "";
  anum_t count, first, last;

  /* use GROUP command to poll newsgroup */
  if (nntp_query(nntp_data, buf, sizeof(buf)) < 0)
    return -1;
  if (sscanf(buf, "211 " ANUM " " ANUM " " ANUM, &count, &first, &last) != 3)
    return 0;
  if (first == nntp_data->first_message && last == nntp_data->last_message)
    return 0;

  /* articles have been renumbered */
  if (last < nntp_data->last_message)
  {
    nntp_data->last_cached = 0;
    if (nntp_data->newsrc_len)
    {
      mutt_mem_realloc(&nntp_data->newsrc_ent, sizeof(struct NewsrcEntry));
      nntp_data->newsrc_len = 1;
      nntp_data->newsrc_ent[0].first = 1;
      nntp_data->newsrc_ent[0].last = 0;
    }
  }
  nntp_data->first_message = first;
  nntp_data->last_message = last;
  if (!update_stat)
    return 1;

  /* update counters */
  else if (!last || (!nntp_data->newsrc_ent && !nntp_data->last_cached))
    nntp_data->unread = count;
  else
    nntp_group_unread_stat(nntp_data);
  return 1;
}