static void cbs_jpeg_free_scan(void *opaque, uint8_t *content)
{
    JPEGRawScan *scan = (JPEGRawScan*)content;
    av_buffer_unref(&scan->data_ref);
    av_freep(&content);
}