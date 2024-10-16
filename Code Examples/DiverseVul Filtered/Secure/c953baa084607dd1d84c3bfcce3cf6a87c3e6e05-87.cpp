static int avif_add_stream(MOVContext *c, int item_id)
{
    MOVStreamContext *sc;
    AVStream *st;
    int item_index = -1;
    for (int i = 0; i < c->avif_info_size; i++)
        if (c->avif_info[i].item_id == item_id) {
            item_index = i;
            break;
        }
    if (item_index < 0)
        return AVERROR_INVALIDDATA;
    st = avformat_new_stream(c->fc, NULL);
    if (!st)
        return AVERROR(ENOMEM);
    st->id = c->fc->nb_streams;
    sc = av_mallocz(sizeof(MOVStreamContext));
    if (!sc)
        return AVERROR(ENOMEM);

    st->priv_data = sc;
    st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codecpar->codec_id = AV_CODEC_ID_AV1;
    sc->ffindex = st->index;
    c->trak_index = st->index;
    st->avg_frame_rate.num = st->avg_frame_rate.den = 1;
    st->time_base.num = st->time_base.den = 1;
    st->nb_frames = 1;
    sc->time_scale = 1;
    sc = st->priv_data;
    sc->pb = c->fc->pb;
    sc->pb_is_copied = 1;

    // Populate the necessary fields used by mov_build_index.
    sc->stsc_count = 1;
    sc->stsc_data = av_malloc_array(1, sizeof(*sc->stsc_data));
    if (!sc->stsc_data)
        return AVERROR(ENOMEM);
    sc->stsc_data[0].first = 1;
    sc->stsc_data[0].count = 1;
    sc->stsc_data[0].id = 1;
    sc->chunk_count = 1;
    sc->chunk_offsets = av_malloc_array(1, sizeof(*sc->chunk_offsets));
    if (!sc->chunk_offsets)
        return AVERROR(ENOMEM);
    sc->sample_count = 1;
    sc->sample_sizes = av_malloc_array(1, sizeof(*sc->sample_sizes));
    if (!sc->sample_sizes)
        return AVERROR(ENOMEM);
    sc->stts_count = 1;
    sc->stts_data = av_malloc_array(1, sizeof(*sc->stts_data));
    if (!sc->stts_data)
        return AVERROR(ENOMEM);
    sc->stts_data[0].count = 1;
    // Not used for still images. But needed by mov_build_index.
    sc->stts_data[0].duration = 0;
    sc->sample_sizes[0] = c->avif_info[item_index].extent_length;
    sc->chunk_offsets[0] = c->avif_info[item_index].extent_offset;

    mov_build_index(c, st);
    return 0;
}