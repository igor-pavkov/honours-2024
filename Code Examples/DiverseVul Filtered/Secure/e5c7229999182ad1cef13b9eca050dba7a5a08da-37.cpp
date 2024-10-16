static void compat_free_buffer(void *opaque, uint8_t *data)
{
    CompatReleaseBufPriv *priv = opaque;
    if (priv->avctx.release_buffer)
        priv->avctx.release_buffer(&priv->avctx, &priv->frame);
    av_freep(&priv);
}