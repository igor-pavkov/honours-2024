static const char* lua_ap_auth_name(request_rec* r)
{
    const char *name;
    name = ap_auth_name(r);
    return name ? name : "";
}