static int vorbis_update_metadata(AVFormatContext *s, int idx)
{
    struct ogg *ogg = s->priv_data;
    struct ogg_stream *os = ogg->streams + idx;
    AVStream *st = s->streams[idx];
    int ret;

    if (os->psize <= 8)
        return 0;

    /* New metadata packet; release old data. */
    av_dict_free(&st->metadata);
    ret = ff_vorbis_comment(s, &st->metadata, os->buf + os->pstart + 7,
                            os->psize - 8);
    if (ret < 0)
        return ret;

    /* Update the metadata if possible. */
    av_freep(&os->new_metadata);
    if (st->metadata) {
        os->new_metadata = av_packet_pack_dictionary(st->metadata, &os->new_metadata_size);
    /* Send an empty dictionary to indicate that metadata has been cleared. */
    } else {
        os->new_metadata = av_malloc(1);
        os->new_metadata_size = 0;
    }

    return ret;
}