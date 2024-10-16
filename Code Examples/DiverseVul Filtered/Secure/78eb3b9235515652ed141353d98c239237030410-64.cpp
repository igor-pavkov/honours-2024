static const char *req_content_encoding_field(request_rec *r)
{
    return r->content_encoding;
}