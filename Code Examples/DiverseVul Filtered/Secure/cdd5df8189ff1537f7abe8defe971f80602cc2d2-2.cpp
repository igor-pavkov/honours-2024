static void flush_fifo(AVFifoBuffer *fifo)
{
    while (av_fifo_size(fifo)) {
        AVFrame *tmp;
        av_fifo_generic_read(fifo, &tmp, sizeof(tmp), NULL);
        av_frame_free(&tmp);
    }
}