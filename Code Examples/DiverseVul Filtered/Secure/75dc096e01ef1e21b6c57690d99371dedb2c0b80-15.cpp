curl_off_t Curl_multi_chunk_length_penalty_size(struct Curl_multi *multi)
{
  return multi ? multi->chunk_length_penalty_size : 0;
}