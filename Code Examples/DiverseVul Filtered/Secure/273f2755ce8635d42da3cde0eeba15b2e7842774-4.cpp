static int html_color_parse(void *log_ctx, const char *str)
{
    uint8_t rgba[4];
    if (av_parse_color(rgba, str, strcspn(str, "\" >"), log_ctx) < 0)
        return -1;
    return rgba[0] | rgba[1] << 8 | rgba[2] << 16;
}