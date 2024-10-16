static int mov_read_timecode_track(AVFormatContext *s, AVStream *st)
{
    MOVStreamContext *sc = st->priv_data;
    FFStream *const sti = ffstream(st);
    int flags = 0;
    int64_t cur_pos = avio_tell(sc->pb);
    int64_t value;
    AVRational tc_rate = st->avg_frame_rate;
    int tmcd_nb_frames = sc->tmcd_nb_frames;
    int rounded_tc_rate;

    if (!sti->nb_index_entries)
        return -1;

    if (!tc_rate.num || !tc_rate.den || !tmcd_nb_frames)
        return -1;

    avio_seek(sc->pb, sti->index_entries->pos, SEEK_SET);
    value = avio_rb32(s->pb);

    if (sc->tmcd_flags & 0x0001) flags |= AV_TIMECODE_FLAG_DROPFRAME;
    if (sc->tmcd_flags & 0x0002) flags |= AV_TIMECODE_FLAG_24HOURSMAX;
    if (sc->tmcd_flags & 0x0004) flags |= AV_TIMECODE_FLAG_ALLOWNEGATIVE;

    /* Assume Counter flag is set to 1 in tmcd track (even though it is likely
     * not the case) and thus assume "frame number format" instead of QT one.
     * No sample with tmcd track can be found with a QT timecode at the moment,
     * despite what the tmcd track "suggests" (Counter flag set to 0 means QT
     * format). */

    /* 60 fps content have tmcd_nb_frames set to 30 but tc_rate set to 60, so
     * we multiply the frame number with the quotient.
     * See tickets #9492, #9710. */
    rounded_tc_rate = (tc_rate.num + tc_rate.den / 2) / tc_rate.den;
    /* Work around files where tmcd_nb_frames is rounded down from frame rate
     * instead of up. See ticket #5978. */
    if (tmcd_nb_frames == tc_rate.num / tc_rate.den &&
        s->strict_std_compliance < FF_COMPLIANCE_STRICT)
        tmcd_nb_frames = rounded_tc_rate;
    value = av_rescale(value, rounded_tc_rate, tmcd_nb_frames);

    parse_timecode_in_framenum_format(s, st, value, flags);

    avio_seek(sc->pb, cur_pos, SEEK_SET);
    return 0;
}