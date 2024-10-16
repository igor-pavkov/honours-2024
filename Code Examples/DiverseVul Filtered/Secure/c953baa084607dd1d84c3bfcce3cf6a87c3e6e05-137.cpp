static int64_t get_stream_info_time(MOVFragmentStreamInfo * frag_stream_info)
{
    av_assert0(frag_stream_info);
    if (frag_stream_info->sidx_pts != AV_NOPTS_VALUE)
        return frag_stream_info->sidx_pts;
    if (frag_stream_info->first_tfra_pts != AV_NOPTS_VALUE)
        return frag_stream_info->first_tfra_pts;
    return frag_stream_info->tfdt_dts;
}