uschar *dkim_exim_sign(int dkim_fd,
                       uschar *dkim_private_key,
                       uschar *dkim_domain,
                       uschar *dkim_selector,
                       uschar *dkim_canon,
                       uschar *dkim_sign_headers) {
  int sep = 0;
  uschar *seen_items = NULL;
  int seen_items_size = 0;
  int seen_items_offset = 0;
  uschar itembuf[256];
  uschar *dkim_canon_expanded;
  uschar *dkim_sign_headers_expanded;
  uschar *dkim_private_key_expanded;
  pdkim_ctx *ctx = NULL;
  uschar *rc = NULL;
  uschar *sigbuf = NULL;
  int sigsize = 0;
  int sigptr = 0;
  pdkim_signature *signature;
  int pdkim_canon;
  int pdkim_rc;
  int sread;
  char buf[4096];
  int save_errno = 0;
  int old_pool = store_pool;

  store_pool = POOL_MAIN;

  dkim_domain = expand_string(dkim_domain);
  if (dkim_domain == NULL) {
    /* expansion error, do not send message. */
    log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand "
          "dkim_domain: %s", expand_string_message);
    rc = NULL;
    goto CLEANUP;
  }

  /* Set $dkim_domain expansion variable to each unique domain in list. */
  while ((dkim_signing_domain = string_nextinlist(&dkim_domain, &sep,
                                                  itembuf,
                                                  sizeof(itembuf))) != NULL) {
    if (!dkim_signing_domain || (dkim_signing_domain[0] == '\0')) continue;
    /* Only sign once for each domain, no matter how often it
       appears in the expanded list. */
    if (seen_items != NULL) {
      uschar *seen_items_list = seen_items;
      if (match_isinlist(dkim_signing_domain,
                         &seen_items_list,0,NULL,NULL,MCL_STRING,TRUE,NULL) == OK)
        continue;
      seen_items = string_append(seen_items,&seen_items_size,&seen_items_offset,1,":");
    }
    seen_items = string_append(seen_items,&seen_items_size,&seen_items_offset,1,dkim_signing_domain);
    seen_items[seen_items_offset] = '\0';

    /* Set up $dkim_selector expansion variable. */
    dkim_signing_selector = expand_string(dkim_selector);
    if (dkim_signing_selector == NULL) {
      log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand "
                "dkim_selector: %s", expand_string_message);
      rc = NULL;
      goto CLEANUP;
    }

    /* Get canonicalization to use */
    dkim_canon_expanded = expand_string(dkim_canon?dkim_canon:US"relaxed");
    if (dkim_canon_expanded == NULL) {
      /* expansion error, do not send message. */
      log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand "
                "dkim_canon: %s", expand_string_message);
      rc = NULL;
      goto CLEANUP;
    }
    if (Ustrcmp(dkim_canon_expanded, "relaxed") == 0)
      pdkim_canon = PDKIM_CANON_RELAXED;
    else if (Ustrcmp(dkim_canon_expanded, "simple") == 0)
      pdkim_canon = PDKIM_CANON_SIMPLE;
    else {
      log_write(0, LOG_MAIN, "DKIM: unknown canonicalization method '%s', defaulting to 'relaxed'.\n",dkim_canon_expanded);
      pdkim_canon = PDKIM_CANON_RELAXED;
    }

    if (dkim_sign_headers) {
      dkim_sign_headers_expanded = expand_string(dkim_sign_headers);
      if (dkim_sign_headers_expanded == NULL) {
        log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand "
                  "dkim_sign_headers: %s", expand_string_message);
        rc = NULL;
        goto CLEANUP;
      }
    }
    else {
      /* pass NULL, which means default header list */
      dkim_sign_headers_expanded = NULL;
    }

    /* Get private key to use. */
    dkim_private_key_expanded = expand_string(dkim_private_key);
    if (dkim_private_key_expanded == NULL) {
      log_write(0, LOG_MAIN|LOG_PANIC, "failed to expand "
                "dkim_private_key: %s", expand_string_message);
      rc = NULL;
      goto CLEANUP;
    }
    if ( (Ustrlen(dkim_private_key_expanded) == 0) ||
         (Ustrcmp(dkim_private_key_expanded,"0") == 0) ||
         (Ustrcmp(dkim_private_key_expanded,"false") == 0) ) {
      /* don't sign, but no error */
      continue;
    }

    if (dkim_private_key_expanded[0] == '/') {
      int privkey_fd = 0;
      /* Looks like a filename, load the private key. */
      memset(big_buffer,0,big_buffer_size);
      privkey_fd = open(CS dkim_private_key_expanded,O_RDONLY);
      if (privkey_fd < 0) {
        log_write(0, LOG_MAIN|LOG_PANIC, "unable to open "
                  "private key file for reading: %s", dkim_private_key_expanded);
        rc = NULL;
        goto CLEANUP;
      }
      (void)read(privkey_fd,big_buffer,(big_buffer_size-2));
      (void)close(privkey_fd);
      dkim_private_key_expanded = big_buffer;
    }

    ctx = pdkim_init_sign(PDKIM_INPUT_SMTP,
                          (char *)dkim_signing_domain,
                          (char *)dkim_signing_selector,
                          (char *)dkim_private_key_expanded
                         );

    pdkim_set_debug_stream(ctx,debug_file);

    pdkim_set_optional(ctx,
                       (char *)dkim_sign_headers_expanded,
                       NULL,
                       pdkim_canon,
                       pdkim_canon,
                       -1,
                       PDKIM_ALGO_RSA_SHA256,
                       0,
                       0);

    lseek(dkim_fd, 0, SEEK_SET);
    while((sread = read(dkim_fd,&buf,4096)) > 0) {
      if (pdkim_feed(ctx,buf,sread) != PDKIM_OK) {
        rc = NULL;
        goto CLEANUP;
      }
    }
    /* Handle failed read above. */
    if (sread == -1) {
      debug_printf("DKIM: Error reading -K file.\n");
      save_errno = errno;
      rc = NULL;
      goto CLEANUP;
    }

    pdkim_rc = pdkim_feed_finish(ctx,&signature);
    if (pdkim_rc != PDKIM_OK) {
      log_write(0, LOG_MAIN|LOG_PANIC, "DKIM: signing failed (RC %d)", pdkim_rc);
      rc = NULL;
      goto CLEANUP;
    }

    sigbuf = string_append(sigbuf, &sigsize, &sigptr, 2,
                           US signature->signature_header,
                           US"\r\n");

    pdkim_free_ctx(ctx);
    ctx = NULL;
  }

  if (sigbuf != NULL) {
    sigbuf[sigptr] = '\0';
    rc = sigbuf;
  } else
    rc = US"";

  CLEANUP:
  if (ctx != NULL)
    pdkim_free_ctx(ctx);
  store_pool = old_pool;
  errno = save_errno;
  return rc;
}