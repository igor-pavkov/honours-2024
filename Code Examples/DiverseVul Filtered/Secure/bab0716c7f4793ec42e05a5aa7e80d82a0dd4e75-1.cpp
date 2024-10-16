static void mxf_compute_edit_units_per_packet(MXFContext *mxf, AVStream *st)
{
    MXFTrack *track = st->priv_data;
    MXFIndexTable *t;

    if (!track)
        return;
    track->edit_units_per_packet = 1;
    if (track->wrapping != ClipWrapped)
        return;

    t = mxf_find_index_table(mxf, track->index_sid);

    /* expect PCM with exactly one index table segment and a small (< 32) EUBC */
    if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO         ||
        !is_pcm(st->codecpar->codec_id)                        ||
        !t                                                     ||
        t->nb_segments != 1                                    ||
        t->segments[0]->edit_unit_byte_count >= 32)
        return;

    /* arbitrarily default to 48 kHz PAL audio frame size */
    /* TODO: We could compute this from the ratio between the audio
     *       and video edit rates for 48 kHz NTSC we could use the
     *       1802-1802-1802-1802-1801 pattern. */
    track->edit_units_per_packet = FFMAX(1, track->edit_rate.num / track->edit_rate.den / 25);
}