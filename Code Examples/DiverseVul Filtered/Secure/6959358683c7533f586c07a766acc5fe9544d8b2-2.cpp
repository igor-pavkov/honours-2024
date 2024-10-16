static int save_avio_options(AVFormatContext *s)
{
    HLSContext *c = s->priv_data;
    const char *opts[] = { "headers", "user_agent", "user-agent", "cookies", NULL }, **opt = opts;
    uint8_t *buf;
    int ret = 0;

    while (*opt) {
        if (av_opt_get(s->pb, *opt, AV_OPT_SEARCH_CHILDREN, &buf) >= 0) {
            ret = av_dict_set(&c->avio_opts, *opt, buf,
                              AV_DICT_DONT_STRDUP_VAL);
            if (ret < 0)
                return ret;
        }
        opt++;
    }

    return ret;
}