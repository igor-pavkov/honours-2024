static int mov_write_trak_tag(AVFormatContext *s, AVIOContext *pb, MOVMuxContext *mov,
                              MOVTrack *track, AVStream *st)
{
    int64_t pos = avio_tell(pb);
    int entry_backup = track->entry;
    int chunk_backup = track->chunkCount;
    int ret;

    /* If we want to have an empty moov, but some samples already have been
     * buffered (delay_moov), pretend that no samples have been written yet. */
    if (mov->flags & FF_MOV_FLAG_EMPTY_MOOV)
        track->chunkCount = track->entry = 0;

    avio_wb32(pb, 0); /* size */
    ffio_wfourcc(pb, "trak");
    mov_write_tkhd_tag(pb, mov, track, st);

    av_assert2(mov->use_editlist >= 0);

    if (track->start_dts != AV_NOPTS_VALUE) {
        if (mov->use_editlist)
            mov_write_edts_tag(pb, mov, track);  // PSP Movies and several other cases require edts box
        else if ((track->entry && track->cluster[0].dts) || track->mode == MODE_PSP || is_clcp_track(track))
            av_log(mov->fc, AV_LOG_WARNING,
                   "Not writing any edit list even though one would have been required\n");
    }

    if (track->tref_tag)
        mov_write_tref_tag(pb, track);

    if ((ret = mov_write_mdia_tag(s, pb, mov, track)) < 0)
        return ret;
    if (track->mode == MODE_PSP)
        mov_write_uuid_tag_psp(pb, track); // PSP Movies require this uuid box
    if (track->tag == MKTAG('r','t','p',' '))
        mov_write_udta_sdp(pb, track);
    if (track->mode == MODE_MOV) {
        if (track->par->codec_type == AVMEDIA_TYPE_VIDEO) {
            double sample_aspect_ratio = av_q2d(st->sample_aspect_ratio);
            if (st->sample_aspect_ratio.num && 1.0 != sample_aspect_ratio) {
                mov_write_tapt_tag(pb, track);
            }
        }
        if (is_clcp_track(track) && st->sample_aspect_ratio.num) {
            mov_write_tapt_tag(pb, track);
        }
    }
    mov_write_track_udta_tag(pb, mov, st);
    track->entry = entry_backup;
    track->chunkCount = chunk_backup;
    return update_size(pb, pos);
}