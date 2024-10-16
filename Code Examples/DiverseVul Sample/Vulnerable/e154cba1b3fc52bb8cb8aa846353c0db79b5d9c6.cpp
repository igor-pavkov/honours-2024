static int fetch_uidl (char *line, void *data)
{
  int i, index;
  CONTEXT *ctx = (CONTEXT *)data;
  POP_DATA *pop_data = (POP_DATA *)ctx->data;
  char *endp;

  errno = 0;
  index = strtol(line, &endp, 10);
  if (errno)
      return -1;
  while (*endp == ' ')
      endp++;
  memmove(line, endp, strlen(endp) + 1);

  for (i = 0; i < ctx->msgcount; i++)
    if (!mutt_strcmp (line, ctx->hdrs[i]->data))
      break;

  if (i == ctx->msgcount)
  {
    dprint (1, (debugfile, "pop_fetch_headers: new header %d %s\n", index, line));

    if (i >= ctx->hdrmax)
      mx_alloc_memory(ctx);

    ctx->msgcount++;
    ctx->hdrs[i] = mutt_new_header ();
    ctx->hdrs[i]->data = safe_strdup (line);
  }
  else if (ctx->hdrs[i]->index != index - 1)
    pop_data->clear_cache = 1;

  ctx->hdrs[i]->refno = index;
  ctx->hdrs[i]->index = index - 1;

  return 0;
}