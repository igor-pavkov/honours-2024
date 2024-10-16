static size_t curl_header_cb(void *ptr, size_t size, size_t nmemb, void *opaque)
{
    BDRVCURLState *s = opaque;
    size_t realsize = size * nmemb;
    const char *accept_line = "Accept-Ranges: bytes";

    if (realsize >= strlen(accept_line)
        && strncmp((char *)ptr, accept_line, strlen(accept_line)) == 0) {
        s->accept_range = true;
    }

    return realsize;
}