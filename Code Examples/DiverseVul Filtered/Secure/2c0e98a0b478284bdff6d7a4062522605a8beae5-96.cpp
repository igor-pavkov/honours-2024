static uint32_t rgb_to_yuv(uint32_t rgb)
{
    uint8_t r, g, b;
    int y, cb, cr;

    r = (rgb >> 16) & 0xFF;
    g = (rgb >>  8) & 0xFF;
    b = (rgb      ) & 0xFF;

    y  = av_clip_uint8(( 16000 +  257 * r + 504 * g +  98 * b)/1000);
    cb = av_clip_uint8((128000 -  148 * r - 291 * g + 439 * b)/1000);
    cr = av_clip_uint8((128000 +  439 * r - 368 * g -  71 * b)/1000);

    return (y << 16) | (cr << 8) | cb;
}