static const char* lua_ap_basic_auth_pw(request_rec* r) 
{
    const char* pw = NULL;
    ap_get_basic_auth_pw(r, &pw);
    return pw ? pw : "";
}