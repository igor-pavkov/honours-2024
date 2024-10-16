AP_DECLARE(const char *)ap_make_content_type(request_rec *r, const char *type)
{
    const apr_strmatch_pattern **pcset;
    core_dir_config *conf =
        (core_dir_config *)ap_get_core_module_config(r->per_dir_config);
    core_request_config *request_conf;
    apr_size_t type_len;

    if (!type || *type == '\0') {
        return NULL;
    }

    if (conf->add_default_charset != ADD_DEFAULT_CHARSET_ON) {
        return type;
    }

    request_conf = ap_get_core_module_config(r->request_config);
    if (request_conf->suppress_charset) {
        return type;
    }

    type_len = strlen(type);

    if (apr_strmatch(charset_pattern, type, type_len) != NULL) {
        /* already has parameter, do nothing */
        /* XXX we don't check the validity */
        ;
    }
    else {
        /* see if it makes sense to add the charset. At present,
         * we only add it if the Content-type is one of needcset[]
         */
        for (pcset = needcset_patterns; *pcset ; pcset++) {
            if (apr_strmatch(*pcset, type, type_len) != NULL) {
                struct iovec concat[3];
                concat[0].iov_base = (void *)type;
                concat[0].iov_len = type_len;
                concat[1].iov_base = (void *)"; charset=";
                concat[1].iov_len = sizeof("; charset=") - 1;
                concat[2].iov_base = (void *)(conf->add_default_charset_name);
                concat[2].iov_len = strlen(conf->add_default_charset_name);
                type = apr_pstrcatv(r->pool, concat, 3, NULL);
                break;
            }
        }
    }

    return type;
}