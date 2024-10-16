static CURLcode tftp_setup_connection(struct connectdata * conn)
{
  struct Curl_easy *data = conn->data;
  char *type;

  conn->transport = TRNSPRT_UDP;

  /* TFTP URLs support an extension like ";mode=<typecode>" that
   * we'll try to get now! */
  type = strstr(data->state.up.path, ";mode=");

  if(!type)
    type = strstr(conn->host.rawalloc, ";mode=");

  if(type) {
    char command;
    *type = 0;                   /* it was in the middle of the hostname */
    command = Curl_raw_toupper(type[6]);

    switch(command) {
    case 'A': /* ASCII mode */
    case 'N': /* NETASCII mode */
      data->set.prefer_ascii = TRUE;
      break;

    case 'O': /* octet mode */
    case 'I': /* binary mode */
    default:
      /* switch off ASCII */
      data->set.prefer_ascii = FALSE;
      break;
    }
  }

  return CURLE_OK;
}