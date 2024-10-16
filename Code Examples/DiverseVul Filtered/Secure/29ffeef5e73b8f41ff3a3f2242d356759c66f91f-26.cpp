static int init_dimensions(H264Context *h)
{
    int width  = h->width  - (h->sps.crop_right + h->sps.crop_left);
    int height = h->height - (h->sps.crop_top   + h->sps.crop_bottom);
    av_assert0(h->sps.crop_right + h->sps.crop_left < (unsigned)h->width);
    av_assert0(h->sps.crop_top + h->sps.crop_bottom < (unsigned)h->height);

    /* handle container cropping */
    if (!h->sps.crop &&
        FFALIGN(h->avctx->width,  16) == h->width &&
        FFALIGN(h->avctx->height, 16) == h->height) {
        width  = h->avctx->width;
        height = h->avctx->height;
    }

    if (width <= 0 || height <= 0) {
        av_log(h->avctx, AV_LOG_ERROR, "Invalid cropped dimensions: %dx%d.\n",
               width, height);
        if (h->avctx->err_recognition & AV_EF_EXPLODE)
            return AVERROR_INVALIDDATA;

        av_log(h->avctx, AV_LOG_WARNING, "Ignoring cropping information.\n");
        h->sps.crop_bottom = h->sps.crop_top = h->sps.crop_right = h->sps.crop_left = 0;
        h->sps.crop        = 0;

        width  = h->width;
        height = h->height;
    }

    h->avctx->coded_width  = h->width;
    h->avctx->coded_height = h->height;
    h->avctx->width        = width;
    h->avctx->height       = height;

    return 0;
}