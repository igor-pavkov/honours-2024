static void compat_release_buffer(void *opaque, uint8_t *data)
{
    AVBufferRef *buf = opaque;
    av_buffer_unref(&buf);
}