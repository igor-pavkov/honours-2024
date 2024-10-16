static void cbs_jpeg_close(CodedBitstreamContext *ctx)
{
    CodedBitstreamJPEGContext *priv = ctx->priv_data;

    av_freep(&priv->write_buffer);
}