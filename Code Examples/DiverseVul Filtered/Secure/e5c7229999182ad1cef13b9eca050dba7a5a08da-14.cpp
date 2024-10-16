void avcodec_get_frame_defaults(AVFrame *frame)
{
#if LIBAVCODEC_VERSION_MAJOR >= 55
     // extended_data should explicitly be freed when needed, this code is unsafe currently
     // also this is not compatible to the <55 ABI/API
    if (frame->extended_data != frame->data && 0)
        av_freep(&frame->extended_data);
#endif

    memset(frame, 0, sizeof(AVFrame));
    av_frame_unref(frame);
}