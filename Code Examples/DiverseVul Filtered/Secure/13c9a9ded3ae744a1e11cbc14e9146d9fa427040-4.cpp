static CURLcode imap_parse_custom_request(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  struct IMAP *imap = data->req.protop;
  const char *custom = data->set.str[STRING_CUSTOMREQUEST];

  if(custom) {
    /* URL decode the custom request */
    result = Curl_urldecode(data, custom, 0, &imap->custom, NULL, TRUE);

    /* Extract the parameters if specified */
    if(!result) {
      const char *params = imap->custom;

      while(*params && *params != ' ')
        params++;

      if(*params) {
        imap->custom_params = strdup(params);
        imap->custom[params - imap->custom] = '\0';

        if(!imap->custom_params)
          result = CURLE_OUT_OF_MEMORY;
      }
    }
  }

  return result;
}