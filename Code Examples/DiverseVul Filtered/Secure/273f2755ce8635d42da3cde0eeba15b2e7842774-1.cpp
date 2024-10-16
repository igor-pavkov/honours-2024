static void rstrip_spaces_buf(AVBPrint *buf)
{
    if (av_bprint_is_complete(buf))
        while (buf->len > 0 && buf->str[buf->len - 1] == ' ')
            buf->str[--buf->len] = 0;
}