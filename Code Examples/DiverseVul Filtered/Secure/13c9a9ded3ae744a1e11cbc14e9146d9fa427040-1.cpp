static void imap_to_imaps(struct connectdata *conn)
{
  /* Change the connection handler */
  conn->handler = &Curl_handler_imaps;

  /* Set the connection's upgraded to TLS flag */
  conn->tls_upgraded = TRUE;
}