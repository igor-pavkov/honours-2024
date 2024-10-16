static CURLcode imap_setup_connection(struct connectdata *conn)
{
  struct Curl_easy *data = conn->data;

  /* Initialise the IMAP layer */
  CURLcode result = imap_init(conn);
  if(result)
    return result;

  /* Clear the TLS upgraded flag */
  conn->tls_upgraded = FALSE;
  data->state.path++;   /* don't include the initial slash */

  return CURLE_OK;
}