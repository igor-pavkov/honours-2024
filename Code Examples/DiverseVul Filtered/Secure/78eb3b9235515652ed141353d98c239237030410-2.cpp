static int lua_ap_limit_req_body(request_rec* r) 
{
    return (int) ap_get_limit_req_body(r);
}