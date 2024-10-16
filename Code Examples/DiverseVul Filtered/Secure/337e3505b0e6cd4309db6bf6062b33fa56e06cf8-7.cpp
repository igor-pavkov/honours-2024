void dkim_exim_verify_init(void) {

  /* Free previous context if there is one */
  if (dkim_verify_ctx) pdkim_free_ctx(dkim_verify_ctx);

  /* Create new context */
  dkim_verify_ctx = pdkim_init_verify(PDKIM_INPUT_SMTP,
                                      &dkim_exim_query_dns_txt
                                     );

  if (dkim_verify_ctx != NULL) {
    dkim_collect_input = TRUE;
    pdkim_set_debug_stream(dkim_verify_ctx,debug_file);
  }
  else dkim_collect_input = FALSE;

}