void avcodec_default_release_buffer(AVCodecContext *s, AVFrame *pic)
{
    av_assert0(s->codec_type == AVMEDIA_TYPE_VIDEO);

    av_frame_unref(pic);
}