static int64_t get_frag_time(AVFormatContext *s, AVStream *dst_st,
                             MOVFragmentIndex *frag_index, int index)
{
    MOVFragmentStreamInfo * frag_stream_info;
    MOVStreamContext *sc = dst_st->priv_data;
    int64_t timestamp;
    int i, j;

    // If the stream is referenced by any sidx, limit the search
    // to fragments that referenced this stream in the sidx
    if (sc->has_sidx) {
        frag_stream_info = get_frag_stream_info(frag_index, index, dst_st->id);
        if (frag_stream_info->sidx_pts != AV_NOPTS_VALUE)
            return frag_stream_info->sidx_pts;
        if (frag_stream_info->first_tfra_pts != AV_NOPTS_VALUE)
            return frag_stream_info->first_tfra_pts;
        return frag_stream_info->sidx_pts;
    }

    for (i = 0; i < frag_index->item[index].nb_stream_info; i++) {
        AVStream *frag_stream = NULL;
        frag_stream_info = &frag_index->item[index].stream_info[i];
        for (j = 0; j < s->nb_streams; j++)
            if (s->streams[j]->id == frag_stream_info->id)
                frag_stream = s->streams[j];
        if (!frag_stream) {
            av_log(s, AV_LOG_WARNING, "No stream matching sidx ID found.\n");
            continue;
        }
        timestamp = get_stream_info_time(frag_stream_info);
        if (timestamp != AV_NOPTS_VALUE)
            return av_rescale_q(timestamp, frag_stream->time_base, dst_st->time_base);
    }
    return AV_NOPTS_VALUE;
}