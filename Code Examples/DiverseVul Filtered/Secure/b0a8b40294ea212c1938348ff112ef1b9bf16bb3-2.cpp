static av_cold int decode_end(AVCodecContext *avctx)
{
    EXRContext *s = avctx->priv_data;
    int i;
    for (i = 0; i < avctx->thread_count; i++) {
        EXRThreadData *td = &s->thread_data[i];
        av_freep(&td->uncompressed_data);
        av_freep(&td->tmp);
        av_freep(&td->bitmap);
        av_freep(&td->lut);
    }

    av_freep(&s->thread_data);
    av_freep(&s->channels);

    return 0;
}