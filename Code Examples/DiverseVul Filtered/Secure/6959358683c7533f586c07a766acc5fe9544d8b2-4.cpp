static int open_url(HLSContext *c, URLContext **uc, const char *url, AVDictionary *opts)
{
    AVDictionary *tmp = NULL;
    int ret;
    const char *proto_name = avio_find_protocol_name(url);

    if (!proto_name)
        return AVERROR_INVALIDDATA;

    // only http(s) & file are allowed
    if (av_strstart(proto_name, "file", NULL)) {
        if (strcmp(c->allowed_extensions, "ALL") && !av_match_ext(url, c->allowed_extensions)) {
            av_log(c, AV_LOG_ERROR,
                "Filename extension of \'%s\' is not a common multimedia extension, blocked for security reasons.\n"
                "If you wish to override this adjust allowed_extensions, you can set it to \'ALL\' to allow all\n",
                url);
            return AVERROR_INVALIDDATA;
        }
    } else if (av_strstart(proto_name, "http", NULL)) {
        ;
    } else
        return AVERROR_INVALIDDATA;

    if (!strncmp(proto_name, url, strlen(proto_name)) && url[strlen(proto_name)] == ':')
        ;
    else if (strcmp(proto_name, "file") || !strncmp(url, "file,", 5))
        return AVERROR_INVALIDDATA;

    av_dict_copy(&tmp, c->avio_opts, 0);
    av_dict_copy(&tmp, opts, 0);

    ret = ffurl_open(uc, url, AVIO_FLAG_READ, c->interrupt_callback, &tmp);
    if( ret >= 0) {
        // update cookies on http response with setcookies.
        URLContext *u = *uc;
        update_options(&c->cookies, "cookies", u->priv_data);
        av_dict_set(&opts, "cookies", c->cookies, 0);
    }

    av_dict_free(&tmp);

    return ret;
}