static CURLcode imap_init(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  struct IMAP *imap;

  imap = data->req.protop = calloc(sizeof(struct IMAP), 1);
  if(!imap)
    result = CURLE_OUT_OF_MEMORY;

  return result;
}