static int mov_read_tfhd(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVFragment *frag = &c->fragment;
    MOVTrackExt *trex = NULL;
    int flags, track_id, i;
    MOVFragmentStreamInfo * frag_stream_info;

    avio_r8(pb); /* version */
    flags = avio_rb24(pb);

    track_id = avio_rb32(pb);
    if (!track_id)
        return AVERROR_INVALIDDATA;
    for (i = 0; i < c->trex_count; i++)
        if (c->trex_data[i].track_id == track_id) {
            trex = &c->trex_data[i];
            break;
        }
    if (!trex) {
        av_log(c->fc, AV_LOG_WARNING, "could not find corresponding trex (id %u)\n", track_id);
        return 0;
    }
    c->fragment.found_tfhd = 1;
    frag->track_id = track_id;
    set_frag_stream(&c->frag_index, track_id);

    frag->base_data_offset = flags & MOV_TFHD_BASE_DATA_OFFSET ?
                             avio_rb64(pb) : flags & MOV_TFHD_DEFAULT_BASE_IS_MOOF ?
                             frag->moof_offset : frag->implicit_offset;
    frag->stsd_id  = flags & MOV_TFHD_STSD_ID ? avio_rb32(pb) : trex->stsd_id;

    frag->duration = flags & MOV_TFHD_DEFAULT_DURATION ?
                     avio_rb32(pb) : trex->duration;
    frag->size     = flags & MOV_TFHD_DEFAULT_SIZE ?
                     avio_rb32(pb) : trex->size;
    frag->flags    = flags & MOV_TFHD_DEFAULT_FLAGS ?
                     avio_rb32(pb) : trex->flags;
    av_log(c->fc, AV_LOG_TRACE, "frag flags 0x%x\n", frag->flags);

    frag_stream_info = get_current_frag_stream_info(&c->frag_index);
    if (frag_stream_info)
        frag_stream_info->next_trun_dts = AV_NOPTS_VALUE;

    return 0;
}