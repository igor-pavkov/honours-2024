static CURLMcode multi_addmsg(struct Curl_multi *multi,
                              struct Curl_message *msg)
{
  if(!Curl_llist_insert_next(multi->msglist, multi->msglist->tail, msg))
    return CURLM_OUT_OF_MEMORY;

  return CURLM_OK;
}