Curl_send_buffer *Curl_add_buffer_init(void)
{
  return calloc(1, sizeof(Curl_send_buffer));
}