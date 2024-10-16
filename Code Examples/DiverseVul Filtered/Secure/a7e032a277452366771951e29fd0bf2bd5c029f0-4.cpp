static inline void get_strl(AVIOContext *pb, char *buf, int buf_size, int len)
{
    int read = avio_get_str(pb, len, buf, buf_size);

    if (read > 0)
        avio_skip(pb, len - read);
}