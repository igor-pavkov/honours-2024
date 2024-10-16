static void nntp_parse_xref(struct Context *ctx, struct Header *hdr)
{
  struct NntpData *nntp_data = ctx->data;

  char *buf = mutt_str_strdup(hdr->env->xref);
  char *p = buf;
  while (p)
  {
    anum_t anum;

    /* skip to next word */
    p += strspn(p, " \t");
    char *grp = p;

    /* skip to end of word */
    p = strpbrk(p, " \t");
    if (p)
      *p++ = '\0';

    /* find colon */
    char *colon = strchr(grp, ':');
    if (!colon)
      continue;
    *colon++ = '\0';
    if (sscanf(colon, ANUM, &anum) != 1)
      continue;

    nntp_article_status(ctx, hdr, grp, anum);
    if (!NHDR(hdr)->article_num && (mutt_str_strcmp(nntp_data->group, grp) == 0))
      NHDR(hdr)->article_num = anum;
  }
  FREE(&buf);
}