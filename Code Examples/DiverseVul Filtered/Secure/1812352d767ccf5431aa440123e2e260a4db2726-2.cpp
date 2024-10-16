static void cbs_jpeg_free_comment(void *opaque, uint8_t *content)
{
    JPEGRawComment *comment = (JPEGRawComment*)content;
    av_buffer_unref(&comment->Cm_ref);
    av_freep(&content);
}