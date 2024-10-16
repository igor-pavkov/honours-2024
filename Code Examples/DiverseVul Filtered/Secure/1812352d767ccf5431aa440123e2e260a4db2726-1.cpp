static void cbs_jpeg_free_application_data(void *opaque, uint8_t *content)
{
    JPEGRawApplicationData *ad = (JPEGRawApplicationData*)content;
    av_buffer_unref(&ad->Ap_ref);
    av_freep(&content);
}