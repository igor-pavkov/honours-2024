void dkim_exim_acl_setup(uschar *id) {
  pdkim_signature *sig = dkim_signatures;
  dkim_cur_sig = NULL;
  dkim_cur_signer = id;
  if (dkim_disable_verify ||
      !id || !dkim_verify_ctx) return;
  /* Find signature to run ACL on */
  while (sig != NULL) {
    uschar *cmp_val = NULL;
    if (Ustrchr(id,'@') != NULL) cmp_val = (uschar *)sig->identity;
                            else cmp_val = (uschar *)sig->domain;
    if (cmp_val && (strcmpic(cmp_val,id) == 0)) {
      dkim_cur_sig = sig;
      /* The "dkim_domain" and "dkim_selector" expansion variables have
         related globals, since they are used in the signing code too.
         Instead of inventing separate names for verification, we set
         them here. This is easy since a domain and selector is guaranteed
         to be in a signature. The other dkim_* expansion items are
         dynamically fetched from dkim_cur_sig at expansion time (see
         function below). */
      dkim_signing_domain   = (uschar *)sig->domain;
      dkim_signing_selector = (uschar *)sig->selector;
      return;
    }
    sig = sig->next;
  }
}