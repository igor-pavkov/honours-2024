void dkim_exim_verify_feed(uschar *data, int len) {
  if (dkim_collect_input &&
      pdkim_feed(dkim_verify_ctx,
                 (char *)data,
                 len) != PDKIM_OK) dkim_collect_input = FALSE;
}