int imap_reconnect (IMAP_DATA **p_idata)
{
  CONTEXT *orig_ctx, new_ctx;
  int rc = -1, i;
  IMAP_DATA *idata = *p_idata;
  HEADER *old_hdr, *new_hdr;

  /* L10N:
     Message displayed when IMAP connection is lost and Mutt
     tries to reconnect.
  */
  mutt_message _("Trying to reconnect...");
  mutt_sleep (0);

  orig_ctx = idata->ctx;
  if (!orig_ctx)
    goto cleanup;

  if (mx_open_mailbox (orig_ctx->path,
                       orig_ctx->readonly ? MUTT_READONLY : 0,
                       &new_ctx) == NULL)
    goto cleanup;

  new_ctx.dontwrite = orig_ctx->dontwrite;
  new_ctx.pattern = orig_ctx->pattern;
  new_ctx.limit_pattern = orig_ctx->limit_pattern;

  orig_ctx->pattern = NULL;
  orig_ctx->limit_pattern = NULL;

  if (idata->uid_validity == ((IMAP_DATA *) new_ctx.data)->uid_validity)
  {
    for (i = 0; i < new_ctx.msgcount; i++)
    {
      new_hdr = new_ctx.hdrs[i];
      old_hdr = (HEADER *) int_hash_find (idata->uid_hash,
                                          HEADER_DATA(new_hdr)->uid);
      if (!old_hdr)
        continue;

      /* this logic is in part from mbox.c. */
      if (old_hdr->changed)
      {
        mutt_set_flag (&new_ctx, new_hdr, MUTT_FLAG, old_hdr->flagged);
        mutt_set_flag (&new_ctx, new_hdr, MUTT_REPLIED, old_hdr->replied);
        mutt_set_flag (&new_ctx, new_hdr, MUTT_OLD, old_hdr->old);
        mutt_set_flag (&new_ctx, new_hdr, MUTT_READ, old_hdr->read);

        /* TODO: the ->env check is unfortunately voodoo that I
         * haven't taken the time to track down yet.  It's in other
         * parts of the code but I don't know why yet. */
        if (old_hdr->env && old_hdr->env->changed)
        {
          new_hdr->env->changed = old_hdr->env->changed;
          new_hdr->changed = 1;
          new_ctx.changed = 1;

          if (old_hdr->env->changed & MUTT_ENV_CHANGED_IRT)
          {
            mutt_free_list (&new_hdr->env->in_reply_to);
            new_hdr->env->in_reply_to = old_hdr->env->in_reply_to;
            old_hdr->env->in_reply_to = NULL;
          }
          if (old_hdr->env->changed & MUTT_ENV_CHANGED_REFS)
          {
            mutt_free_list (&new_hdr->env->references);
            new_hdr->env->references = old_hdr->env->references;
            old_hdr->env->references = NULL;
          }
          if (old_hdr->env->changed & MUTT_ENV_CHANGED_XLABEL)
          {
            FREE (&new_hdr->env->x_label);
            new_hdr->env->x_label = old_hdr->env->x_label;
            old_hdr->env->x_label = NULL;
          }
          if (old_hdr->env->changed & MUTT_ENV_CHANGED_SUBJECT)
          {
            FREE (&new_hdr->env->subject);
            new_hdr->env->subject = old_hdr->env->subject;
            new_hdr->env->real_subj = old_hdr->env->real_subj;
            old_hdr->env->subject = old_hdr->env->real_subj = NULL;
          }
        }

        if (old_hdr->attach_del)
        {
          if (old_hdr->content->parts && !new_hdr->content->parts)
          {
            new_hdr->attach_del = 1;
            new_hdr->changed = 1;
            new_ctx.changed = 1;
            new_hdr->content->parts = old_hdr->content->parts;
            old_hdr->content->parts = NULL;
          }
        }
      }

      mutt_set_flag (&new_ctx, new_hdr, MUTT_DELETE, old_hdr->deleted);
      mutt_set_flag (&new_ctx, new_hdr, MUTT_PURGE, old_hdr->purge);
      mutt_set_flag (&new_ctx, new_hdr, MUTT_TAG, old_hdr->tagged);
    }
  }

  rc = 0;

cleanup:
  idata->status = IMAP_FATAL;
  mx_fastclose_mailbox (orig_ctx);
  imap_close_connection (idata);

  if (rc != 0)
  {
    /* L10N:
       Message when Mutt tries to reconnect to an IMAP mailbox but is
       unable to.
    */
    mutt_error _("Reconnect failed.  Mailbox closed.");
  }
  else
  {
    memcpy (orig_ctx, &new_ctx, sizeof(CONTEXT));
    idata = (IMAP_DATA *)orig_ctx->data;
    idata->ctx = orig_ctx;
    *p_idata = idata;
    /* L10N:
       Message when Mutt reconnects to an IMAP mailbox after a fatal error.
    */
    mutt_error _("Reconnect succeeded.");
  }
  mutt_sleep (0);

  return rc;
}