void ass_frame_unref(ASS_Image *img)
{
    if (!img || --((ASS_ImagePriv *) img)->ref_count)
        return;
    do {
        ASS_ImagePriv *priv = (ASS_ImagePriv *) img;
        img = img->next;
        if (priv->source)
            ass_cache_dec_ref(priv->source);
        else
            ass_aligned_free(priv->result.bitmap);
        free(priv);
    } while (img);
}