int nntp_check_msgid(struct Context *ctx, const char *msgid)
{
  struct NntpData *nntp_data = ctx->data;
  char buf[LONG_STRING];

  FILE *fp = mutt_file_mkstemp();
  if (!fp)
  {
    mutt_perror("mutt_file_mkstemp() failed!");
    return -1;
  }

  snprintf(buf, sizeof(buf), "HEAD %s\r\n", msgid);
  int rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), NULL, fetch_tempfile, fp);
  if (rc)
  {
    mutt_file_fclose(&fp);
    if (rc < 0)
      return -1;
    if (mutt_str_strncmp("430", buf, 3) == 0)
      return 1;
    mutt_error("HEAD: %s", buf);
    return -1;
  }

  /* parse header */
  if (ctx->msgcount == ctx->hdrmax)
    mx_alloc_memory(ctx);
  struct Header *hdr = ctx->hdrs[ctx->msgcount] = mutt_header_new();
  hdr->data = mutt_mem_calloc(1, sizeof(struct NntpHeaderData));
  hdr->env = mutt_rfc822_read_header(fp, hdr, 0, 0);
  mutt_file_fclose(&fp);

  /* get article number */
  if (hdr->env->xref)
    nntp_parse_xref(ctx, hdr);
  else
  {
    snprintf(buf, sizeof(buf), "STAT %s\r\n", msgid);
    if (nntp_query(nntp_data, buf, sizeof(buf)) < 0)
    {
      mutt_header_free(&hdr);
      return -1;
    }
    sscanf(buf + 4, ANUM, &NHDR(hdr)->article_num);
  }

  /* reset flags */
  hdr->read = false;
  hdr->old = false;
  hdr->deleted = false;
  hdr->changed = true;
  hdr->received = hdr->date_sent;
  hdr->index = ctx->msgcount++;
  mx_update_context(ctx, 1);
  return 0;
}