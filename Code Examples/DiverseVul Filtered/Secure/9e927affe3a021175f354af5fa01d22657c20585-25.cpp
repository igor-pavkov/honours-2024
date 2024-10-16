static int fetch_description(char *line, void *data)
{
  struct NntpServer *nserv = data;
  struct NntpData *nntp_data = NULL;
  char *desc = NULL;

  if (!line)
    return 0;

  desc = strpbrk(line, " \t");
  if (desc)
  {
    *desc++ = '\0';
    desc += strspn(desc, " \t");
  }
  else
    desc = strchr(line, '\0');

  nntp_data = mutt_hash_find(nserv->groups_hash, line);
  if (nntp_data && (mutt_str_strcmp(desc, nntp_data->desc) != 0))
  {
    mutt_str_replace(&nntp_data->desc, desc);
    mutt_debug(2, "group: %s, desc: %s\n", line, desc);
  }
  return 0;
}