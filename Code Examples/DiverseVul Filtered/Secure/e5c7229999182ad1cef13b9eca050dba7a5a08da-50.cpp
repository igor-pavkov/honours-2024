int ff_set_dimensions(AVCodecContext *s, int width, int height)
{
    int ret = av_image_check_size(width, height, 0, s);

    if (ret < 0)
        width = height = 0;

    s->coded_width  = width;
    s->coded_height = height;
    s->width        = FF_CEIL_RSHIFT(width,  s->lowres);
    s->height       = FF_CEIL_RSHIFT(height, s->lowres);

    return ret;
}