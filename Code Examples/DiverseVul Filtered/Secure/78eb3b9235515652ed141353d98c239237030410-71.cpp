static const char* lua_ap_get_server_name(request_rec* r)
{
    const char *name;
    name = ap_get_server_name(r);
    return name ? name : "localhost";
}