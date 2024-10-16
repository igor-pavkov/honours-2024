static const char *wsgi_set_user_authoritative(cmd_parms *cmd, void *mconfig,
                                               const char *f)
{
    WSGIDirectoryConfig *dconfig = NULL;
    dconfig = (WSGIDirectoryConfig *)mconfig;

    if (strcasecmp(f, "Off") == 0)
        dconfig->user_authoritative = 0;
    else if (strcasecmp(f, "On") == 0)
        dconfig->user_authoritative = 1;
    else
        return "WSGIUserAuthoritative must be one of: Off | On";

    return NULL;
}