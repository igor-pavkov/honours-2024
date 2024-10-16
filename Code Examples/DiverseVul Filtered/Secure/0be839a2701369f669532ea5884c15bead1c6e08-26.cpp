void free_xbzrle_decoded_buf(void)
{
    g_free(xbzrle_decoded_buf);
    xbzrle_decoded_buf = NULL;
}