static const char *req_path_info_field(request_rec *r)
{
    return r->path_info;
}