int am_has_header(request_rec *r, const char *h, const char *v)
{
    return (am_get_header_attr(r, h, v, NULL) != NULL);
}