struct curl_llist *Curl_multi_pipelining_server_bl(struct Curl_multi *multi)
{
  return multi->pipelining_server_bl;
}