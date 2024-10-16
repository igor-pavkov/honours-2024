static av_cold int decode_init(AVCodecContext *avctx)
{
    VmncContext * const c = avctx->priv_data;

    c->avctx  = avctx;
    c->width  = avctx->width;
    c->height = avctx->height;
    c->bpp    = avctx->bits_per_coded_sample;
    c->bpp2   = c->bpp / 8;

    switch (c->bpp) {
    case 8:
        avctx->pix_fmt = AV_PIX_FMT_PAL8;
        break;
    case 16:
        avctx->pix_fmt = AV_PIX_FMT_RGB555;
        break;
    case 32:
        avctx->pix_fmt = AV_PIX_FMT_RGB32;
        break;
    default:
        av_log(avctx, AV_LOG_ERROR, "Unsupported bitdepth %i\n", c->bpp);
        return AVERROR_INVALIDDATA;
    }

    c->pic = av_frame_alloc();
    if (!c->pic)
        return AVERROR(ENOMEM);

    return 0;
}