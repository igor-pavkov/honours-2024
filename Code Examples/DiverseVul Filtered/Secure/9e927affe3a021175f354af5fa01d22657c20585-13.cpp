static int fetch_children(char *line, void *data)
{
  struct ChildCtx *cc = data;
  anum_t anum;

  if (!line || sscanf(line, ANUM, &anum) != 1)
    return 0;
  for (unsigned int i = 0; i < cc->ctx->msgcount; i++)
    if (NHDR(cc->ctx->hdrs[i])->article_num == anum)
      return 0;
  if (cc->num >= cc->max)
  {
    cc->max *= 2;
    mutt_mem_realloc(&cc->child, sizeof(anum_t) * cc->max);
  }
  cc->child[cc->num++] = anum;
  return 0;
}