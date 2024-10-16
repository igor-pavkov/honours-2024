static int adts_write_header(AVFormatContext *s)
{
    ADTSContext *adts = s->priv_data;

    if (adts->id3v2tag)
        ff_id3v2_write_simple(s, 4, ID3v2_DEFAULT_MAGIC);

    return 0;
}