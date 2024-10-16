static int mov_read_rtmd_track(AVFormatContext *s, AVStream *st)
{
    MOVStreamContext *sc = st->priv_data;
    FFStream *const sti = ffstream(st);
    char buf[AV_TIMECODE_STR_SIZE];
    int64_t cur_pos = avio_tell(sc->pb);
    int hh, mm, ss, ff, drop;

    if (!sti->nb_index_entries)
        return -1;

    avio_seek(sc->pb, sti->index_entries->pos, SEEK_SET);
    avio_skip(s->pb, 13);
    hh = avio_r8(s->pb);
    mm = avio_r8(s->pb);
    ss = avio_r8(s->pb);
    drop = avio_r8(s->pb);
    ff = avio_r8(s->pb);
    snprintf(buf, AV_TIMECODE_STR_SIZE, "%02d:%02d:%02d%c%02d",
             hh, mm, ss, drop ? ';' : ':', ff);
    av_dict_set(&st->metadata, "timecode", buf, 0);

    avio_seek(sc->pb, cur_pos, SEEK_SET);
    return 0;
}