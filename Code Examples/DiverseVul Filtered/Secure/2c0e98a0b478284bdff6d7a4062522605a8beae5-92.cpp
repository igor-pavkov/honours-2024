static int mov_write_trailer(AVFormatContext *s)
{
    MOVMuxContext *mov = s->priv_data;
    AVIOContext *pb = s->pb;
    int res = 0;
    int i;
    int64_t moov_pos;

    if (mov->need_rewrite_extradata) {
        for (i = 0; i < s->nb_streams; i++) {
            MOVTrack *track = &mov->tracks[i];
            AVCodecParameters *par = track->par;

            track->vos_len  = par->extradata_size;
            track->vos_data = av_malloc(track->vos_len);
            if (!track->vos_data)
                return AVERROR(ENOMEM);
            memcpy(track->vos_data, par->extradata, track->vos_len);
        }
        mov->need_rewrite_extradata = 0;
    }

    /*
     * Before actually writing the trailer, make sure that there are no
     * dangling subtitles, that need a terminating sample.
     */
    for (i = 0; i < mov->nb_streams; i++) {
        MOVTrack *trk = &mov->tracks[i];
        if (trk->par->codec_id == AV_CODEC_ID_MOV_TEXT &&
            !trk->last_sample_is_subtitle_end) {
            mov_write_subtitle_end_packet(s, i, trk->track_duration);
            trk->last_sample_is_subtitle_end = 1;
        }
    }

    // If there were no chapters when the header was written, but there
    // are chapters now, write them in the trailer.  This only works
    // when we are not doing fragments.
    if (!mov->chapter_track && !(mov->flags & FF_MOV_FLAG_FRAGMENT)) {
        if (mov->mode & (MODE_MP4|MODE_MOV|MODE_IPOD) && s->nb_chapters) {
            mov->chapter_track = mov->nb_streams++;
            if ((res = mov_create_chapter_track(s, mov->chapter_track)) < 0)
                return res;
        }
    }

    if (!(mov->flags & FF_MOV_FLAG_FRAGMENT)) {
        moov_pos = avio_tell(pb);

        /* Write size of mdat tag */
        if (mov->mdat_size + 8 <= UINT32_MAX) {
            avio_seek(pb, mov->mdat_pos, SEEK_SET);
            avio_wb32(pb, mov->mdat_size + 8);
        } else {
            /* overwrite 'wide' placeholder atom */
            avio_seek(pb, mov->mdat_pos - 8, SEEK_SET);
            /* special value: real atom size will be 64 bit value after
             * tag field */
            avio_wb32(pb, 1);
            ffio_wfourcc(pb, "mdat");
            avio_wb64(pb, mov->mdat_size + 16);
        }
        avio_seek(pb, mov->reserved_moov_size > 0 ? mov->reserved_header_pos : moov_pos, SEEK_SET);

        if (mov->flags & FF_MOV_FLAG_FASTSTART) {
            av_log(s, AV_LOG_INFO, "Starting second pass: moving the moov atom to the beginning of the file\n");
            res = shift_data(s);
            if (res < 0)
                return res;
            avio_seek(pb, mov->reserved_header_pos, SEEK_SET);
            if ((res = mov_write_moov_tag(pb, mov, s)) < 0)
                return res;
        } else if (mov->reserved_moov_size > 0) {
            int64_t size;
            if ((res = mov_write_moov_tag(pb, mov, s)) < 0)
                return res;
            size = mov->reserved_moov_size - (avio_tell(pb) - mov->reserved_header_pos);
            if (size < 8){
                av_log(s, AV_LOG_ERROR, "reserved_moov_size is too small, needed %"PRId64" additional\n", 8-size);
                return AVERROR(EINVAL);
            }
            avio_wb32(pb, size);
            ffio_wfourcc(pb, "free");
            ffio_fill(pb, 0, size - 8);
            avio_seek(pb, moov_pos, SEEK_SET);
        } else {
            if ((res = mov_write_moov_tag(pb, mov, s)) < 0)
                return res;
        }
        res = 0;
    } else {
        mov_auto_flush_fragment(s, 1);
        for (i = 0; i < mov->nb_streams; i++)
           mov->tracks[i].data_offset = 0;
        if (mov->flags & FF_MOV_FLAG_GLOBAL_SIDX) {
            int64_t end;
            av_log(s, AV_LOG_INFO, "Starting second pass: inserting sidx atoms\n");
            res = shift_data(s);
            if (res < 0)
                return res;
            end = avio_tell(pb);
            avio_seek(pb, mov->reserved_header_pos, SEEK_SET);
            mov_write_sidx_tags(pb, mov, -1, 0);
            avio_seek(pb, end, SEEK_SET);
            avio_write_marker(s->pb, AV_NOPTS_VALUE, AVIO_DATA_MARKER_TRAILER);
            mov_write_mfra_tag(pb, mov);
        } else {
            avio_write_marker(s->pb, AV_NOPTS_VALUE, AVIO_DATA_MARKER_TRAILER);
            mov_write_mfra_tag(pb, mov);
        }
    }

    return res;
}