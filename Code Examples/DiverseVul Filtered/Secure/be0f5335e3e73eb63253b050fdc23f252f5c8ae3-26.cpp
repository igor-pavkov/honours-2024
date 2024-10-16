static int send_header(void *data, const char *key, const char *val)
{
    ap_fputstrs(((hdr_ptr*)data)->f, ((hdr_ptr*)data)->bb,
                key, ": ", val, CRLF, NULL);
    return 1;
}