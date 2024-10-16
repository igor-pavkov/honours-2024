static const char *req_content_type_field(request_rec *r)
{
    return r->content_type;
}