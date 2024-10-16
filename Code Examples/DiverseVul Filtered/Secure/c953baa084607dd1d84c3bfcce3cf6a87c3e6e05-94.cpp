static int mov_rewrite_dvd_sub_extradata(AVStream *st)
{
    char buf[256] = {0};
    uint8_t *src = st->codecpar->extradata;
    int i, ret;

    if (st->codecpar->extradata_size != 64)
        return 0;

    if (st->codecpar->width > 0 &&  st->codecpar->height > 0)
        snprintf(buf, sizeof(buf), "size: %dx%d\n",
                 st->codecpar->width, st->codecpar->height);
    av_strlcat(buf, "palette: ", sizeof(buf));

    for (i = 0; i < 16; i++) {
        uint32_t yuv = AV_RB32(src + i * 4);
        uint32_t rgba = yuv_to_rgba(yuv);

        av_strlcatf(buf, sizeof(buf), "%06"PRIx32"%s", rgba, i != 15 ? ", " : "");
    }

    if (av_strlcat(buf, "\n", sizeof(buf)) >= sizeof(buf))
        return 0;

    ret = ff_alloc_extradata(st->codecpar, strlen(buf));
    if (ret < 0)
        return ret;
    memcpy(st->codecpar->extradata, buf, st->codecpar->extradata_size);

    return 0;
}