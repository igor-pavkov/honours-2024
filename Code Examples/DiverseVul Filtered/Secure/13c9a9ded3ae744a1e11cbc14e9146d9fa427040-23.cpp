static CURLcode imap_perform_authentication(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct imap_conn *imapc = &conn->proto.imapc;
  saslprogress progress;

  /* Check if already authenticated OR if there is enough data to authenticate
     with and end the connect phase if we don't */
  if(imapc->preauth ||
     !Curl_sasl_can_authenticate(&imapc->sasl, conn)) {
    state(conn, IMAP_STOP);
    return result;
  }

  /* Calculate the SASL login details */
  result = Curl_sasl_start(&imapc->sasl, conn, imapc->ir_supported, &progress);

  if(!result) {
    if(progress == SASL_INPROGRESS)
      state(conn, IMAP_AUTHENTICATE);
    else if(!imapc->login_disabled && (imapc->preftype & IMAP_TYPE_CLEARTEXT))
      /* Perform clear text authentication */
      result = imap_perform_login(conn);
    else {
      /* Other mechanisms not supported */
      infof(conn->data, "No known authentication mechanisms supported!\n");
      result = CURLE_LOGIN_DENIED;
    }
  }

  return result;
}