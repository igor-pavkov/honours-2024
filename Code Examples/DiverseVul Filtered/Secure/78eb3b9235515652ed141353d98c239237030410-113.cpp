static const char* lua_ap_options(request_rec* r) 
{
    int opts;
    opts = ap_allow_options(r);
    return apr_psprintf(r->pool, "%s %s %s %s %s %s", (opts&OPT_INDEXES) ? "Indexes" : "", (opts&OPT_INCLUDES) ? "Includes" : "", (opts&OPT_SYM_LINKS) ? "FollowSymLinks" : "", (opts&OPT_EXECCGI) ? "ExecCGI" : "", (opts&OPT_MULTI) ? "MultiViews" : "", (opts&OPT_ALL) == OPT_ALL ? "All" : "" );
}