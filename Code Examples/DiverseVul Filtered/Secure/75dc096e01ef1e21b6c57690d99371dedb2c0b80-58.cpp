curl_off_t Curl_multi_content_length_penalty_size(struct Curl_multi *multi)
{
  return multi ? multi->content_length_penalty_size : 0;
}