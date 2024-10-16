static int mov_write_string_data_tag(AVIOContext *pb, const char *data, int lang, int long_style)
{
    if (long_style) {
        int size = 16 + strlen(data);
        avio_wb32(pb, size); /* size */
        ffio_wfourcc(pb, "data");
        avio_wb32(pb, 1);
        avio_wb32(pb, 0);
        avio_write(pb, data, strlen(data));
        return size;
    } else {
        if (!lang)
            lang = ff_mov_iso639_to_lang("und", 1);
        avio_wb16(pb, strlen(data)); /* string length */
        avio_wb16(pb, lang);
        avio_write(pb, data, strlen(data));
        return strlen(data) + 4;
    }
}