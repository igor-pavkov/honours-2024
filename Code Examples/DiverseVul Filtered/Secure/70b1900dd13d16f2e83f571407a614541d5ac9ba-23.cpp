static void main_free(void)
{
  curl_global_cleanup();
#if defined(CURL_DOES_CONVERSIONS) && defined(HAVE_ICONV)
  /* close iconv conversion descriptor */
  if(inbound_cd != (iconv_t)-1)
    iconv_close(inbound_cd);
  if(outbound_cd != (iconv_t)-1)
    iconv_close(outbound_cd);
#endif /* CURL_DOES_CONVERSIONS && HAVE_ICONV */
}