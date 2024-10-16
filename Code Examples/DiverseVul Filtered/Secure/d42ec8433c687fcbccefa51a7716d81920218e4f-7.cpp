static av_cold int decode_init(AVCodecContext *avctx)
{
    AnsiContext *s = avctx->priv_data;
    avctx->pix_fmt = AV_PIX_FMT_PAL8;

    s->frame = av_frame_alloc();
    if (!s->frame)
        return AVERROR(ENOMEM);

    /* defaults */
    s->font        = avpriv_vga16_font;
    s->font_height = 16;
    s->fg          = DEFAULT_FG_COLOR;
    s->bg          = DEFAULT_BG_COLOR;

    if (!avctx->width || !avctx->height)
        ff_set_dimensions(avctx, 80 << 3, 25 << 4);

    return 0;
}