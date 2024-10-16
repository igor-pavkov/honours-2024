static int mpeg4_update_thread_context(AVCodecContext *dst,
                                       const AVCodecContext *src)
{
    Mpeg4DecContext *s = dst->priv_data;
    const Mpeg4DecContext *s1 = src->priv_data;

    int ret = ff_mpeg_update_thread_context(dst, src);

    if (ret < 0)
        return ret;

    memcpy(((uint8_t*)s) + sizeof(MpegEncContext), ((uint8_t*)s1) + sizeof(MpegEncContext), sizeof(Mpeg4DecContext) - sizeof(MpegEncContext));

    return 0;
}