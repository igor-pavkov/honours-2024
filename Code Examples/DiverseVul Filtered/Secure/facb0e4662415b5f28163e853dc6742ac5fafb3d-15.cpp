static CURLcode tftp_option_add(tftp_state_data_t *state, size_t *csize,
                                char *buf, const char *option)
{
  if(( strlen(option) + *csize + 1) > (size_t)state->blksize)
    return CURLE_TFTP_ILLEGAL;
  strcpy(buf, option);
  *csize += strlen(option) + 1;
  return CURLE_OK;
}