static int can_seek_to_key_sample(AVStream *st, int sample, int64_t requested_pts)
{
    MOVStreamContext *sc = st->priv_data;
    FFStream *const sti = ffstream(st);
    int64_t key_sample_dts, key_sample_pts;

    if (st->codecpar->codec_id != AV_CODEC_ID_HEVC)
        return 1;

    if (sample >= sc->sample_offsets_count)
        return 1;

    key_sample_dts = sti->index_entries[sample].timestamp;
    key_sample_pts = key_sample_dts + sc->sample_offsets[sample] + sc->dts_shift;

    /*
     * If the sample needs to be presented before an open key sample, they may
     * not be decodable properly, even though they come after in decoding
     * order.
     */
    if (is_open_key_sample(sc, sample) && key_sample_pts > requested_pts)
        return 0;

    return 1;
}