char *Curl_checkheaders(const struct connectdata *conn,
                        const char *thisheader)
{
  struct curl_slist *head;
  size_t thislen = strlen(thisheader);
  struct SessionHandle *data = conn->data;

  for(head = data->set.headers;head; head=head->next) {
    if(Curl_raw_nequal(head->data, thisheader, thislen))
      return head->data;
  }
  return NULL;
}