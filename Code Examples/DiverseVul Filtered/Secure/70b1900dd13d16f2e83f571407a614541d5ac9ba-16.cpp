char convert_char(curl_infotype infotype, char this_char)
{
/* determine how this specific character should be displayed */
  switch(infotype) {
  case CURLINFO_DATA_IN:
  case CURLINFO_DATA_OUT:
  case CURLINFO_SSL_DATA_IN:
  case CURLINFO_SSL_DATA_OUT:
    /* data, treat as ASCII */
    if ((this_char >= 0x20) && (this_char < 0x7f)) {
      /* printable ASCII hex value: convert to host encoding */
      convert_from_network(&this_char, 1);
    }
    else {
      /* non-printable ASCII, use a replacement character */
      return UNPRINTABLE_CHAR;
    }
    /* fall through to default */
  default:
    /* treat as host encoding */
    if (ISPRINT(this_char)
        &&  (this_char != '\t')
        &&  (this_char != '\r')
        &&  (this_char != '\n')) {
      /* printable characters excluding tabs and line end characters */
      return this_char;
    }
    break;
  }
  /* non-printable, use a replacement character  */
  return UNPRINTABLE_CHAR;
}