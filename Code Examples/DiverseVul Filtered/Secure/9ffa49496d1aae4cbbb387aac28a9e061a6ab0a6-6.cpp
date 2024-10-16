static int adts_write_trailer(AVFormatContext *s)
{
    ADTSContext *adts = s->priv_data;

    if (adts->apetag)
        ff_ape_write_tag(s);

    return 0;
}