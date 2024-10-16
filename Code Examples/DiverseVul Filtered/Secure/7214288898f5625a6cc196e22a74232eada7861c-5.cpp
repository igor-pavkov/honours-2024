CURLcode Curl_get_upload_buffer(struct Curl_easy *data)
{
  if(!data->state.ulbuf) {
    data->state.ulbuf = malloc(data->set.upload_buffer_size);
    if(!data->state.ulbuf)
      return CURLE_OUT_OF_MEMORY;
  }
  return CURLE_OK;
}