static const char* lua_ap_allowoverrides(request_rec* r) 
{
    int opts;
    opts = ap_allow_overrides(r);
    if ( (opts & OR_ALL) == OR_ALL) {
        return "All";
    }
    else if (opts == OR_NONE) {
        return "None";
    }
    return apr_psprintf(r->pool, "%s %s %s %s %s", (opts & OR_LIMIT) ? "Limit" : "", (opts & OR_OPTIONS) ? "Options" : "", (opts & OR_FILEINFO) ? "FileInfo" : "", (opts & OR_AUTHCFG) ? "AuthCfg" : "", (opts & OR_INDEXES) ? "Indexes" : "" );
    
}