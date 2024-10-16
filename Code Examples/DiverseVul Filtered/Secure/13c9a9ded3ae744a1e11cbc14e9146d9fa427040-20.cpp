static CURLcode imap_sendf(struct connectdata *conn, const char *fmt, ...)
{
  CURLcode result = CURLE_OK;
  struct imap_conn *imapc = &conn->proto.imapc;
  char *taggedfmt;
  va_list ap;

  DEBUGASSERT(fmt);

  /* Calculate the next command ID wrapping at 3 digits */
  imapc->cmdid = (imapc->cmdid + 1) % 1000;

  /* Calculate the tag based on the connection ID and command ID */
  snprintf(imapc->resptag, sizeof(imapc->resptag), "%c%03d",
           'A' + curlx_sltosi(conn->connection_id % 26), imapc->cmdid);

  /* Prefix the format with the tag */
  taggedfmt = aprintf("%s %s", imapc->resptag, fmt);
  if(!taggedfmt)
    return CURLE_OUT_OF_MEMORY;

  /* Send the data with the tag */
  va_start(ap, fmt);
  result = Curl_pp_vsendf(&imapc->pp, taggedfmt, ap);
  va_end(ap);

  free(taggedfmt);

  return result;
}