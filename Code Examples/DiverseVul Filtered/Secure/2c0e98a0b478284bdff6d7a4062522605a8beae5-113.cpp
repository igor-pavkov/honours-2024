static int mov_write_header(AVFormatContext *s)
{
    AVIOContext *pb = s->pb;
    MOVMuxContext *mov = s->priv_data;
    AVDictionaryEntry *t, *global_tcr = av_dict_get(s->metadata, "timecode", NULL, 0);
    int i, ret, hint_track = 0, tmcd_track = 0, nb_tracks = s->nb_streams;

    if (mov->mode & (MODE_MP4|MODE_MOV|MODE_IPOD) && s->nb_chapters)
        nb_tracks++;

    if (mov->flags & FF_MOV_FLAG_RTP_HINT) {
        /* Add hint tracks for each audio and video stream */
        hint_track = nb_tracks;
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ||
                st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                nb_tracks++;
            }
        }
    }

    if (mov->mode == MODE_MOV || mov->mode == MODE_MP4)
        tmcd_track = nb_tracks;

    for (i = 0; i < s->nb_streams; i++) {
        int j;
        AVStream *st= s->streams[i];
        MOVTrack *track= &mov->tracks[i];

        /* copy extradata if it exists */
        if (st->codecpar->extradata_size) {
            if (st->codecpar->codec_id == AV_CODEC_ID_DVD_SUBTITLE)
                mov_create_dvd_sub_decoder_specific_info(track, st);
            else if (!TAG_IS_AVCI(track->tag) && st->codecpar->codec_id != AV_CODEC_ID_DNXHD) {
                track->vos_len  = st->codecpar->extradata_size;
                track->vos_data = av_malloc(track->vos_len);
                if (!track->vos_data) {
                    return AVERROR(ENOMEM);
                }
                memcpy(track->vos_data, st->codecpar->extradata, track->vos_len);
            }
        }

        if (st->codecpar->codec_type != AVMEDIA_TYPE_AUDIO ||
            track->par->channel_layout != AV_CH_LAYOUT_MONO)
            continue;

        for (j = 0; j < s->nb_streams; j++) {
            AVStream *stj= s->streams[j];
            MOVTrack *trackj= &mov->tracks[j];
            if (j == i)
                continue;

            if (stj->codecpar->codec_type != AVMEDIA_TYPE_AUDIO ||
                trackj->par->channel_layout != AV_CH_LAYOUT_MONO ||
                trackj->language != track->language ||
                trackj->tag != track->tag
            )
                continue;
            track->multichannel_as_mono++;
        }
    }

    if (!(mov->flags & FF_MOV_FLAG_DELAY_MOOV)) {
        if ((ret = mov_write_identification(pb, s)) < 0)
            return ret;
    }

    if (mov->reserved_moov_size){
        mov->reserved_header_pos = avio_tell(pb);
        if (mov->reserved_moov_size > 0)
            avio_skip(pb, mov->reserved_moov_size);
    }

    if (mov->flags & FF_MOV_FLAG_FRAGMENT) {
        /* If no fragmentation options have been set, set a default. */
        if (!(mov->flags & (FF_MOV_FLAG_FRAG_KEYFRAME |
                            FF_MOV_FLAG_FRAG_CUSTOM)) &&
            !mov->max_fragment_duration && !mov->max_fragment_size)
            mov->flags |= FF_MOV_FLAG_FRAG_KEYFRAME;
    } else {
        if (mov->flags & FF_MOV_FLAG_FASTSTART)
            mov->reserved_header_pos = avio_tell(pb);
        mov_write_mdat_tag(pb, mov);
    }

    ff_parse_creation_time_metadata(s, &mov->time, 1);
    if (mov->time)
        mov->time += 0x7C25B080; // 1970 based -> 1904 based

    if (mov->chapter_track)
        if ((ret = mov_create_chapter_track(s, mov->chapter_track)) < 0)
            return ret;

    if (mov->flags & FF_MOV_FLAG_RTP_HINT) {
        /* Initialize the hint tracks for each audio and video stream */
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO ||
                st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                if ((ret = ff_mov_init_hinting(s, hint_track, i)) < 0)
                    return ret;
                hint_track++;
            }
        }
    }

    if (mov->nb_meta_tmcd) {
        /* Initialize the tmcd tracks */
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            t = global_tcr;

            if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                AVTimecode tc;
                if (!t)
                    t = av_dict_get(st->metadata, "timecode", NULL, 0);
                if (!t)
                    continue;
                if (mov_check_timecode_track(s, &tc, i, t->value) < 0)
                    continue;
                if ((ret = mov_create_timecode_track(s, tmcd_track, i, tc)) < 0)
                    return ret;
                tmcd_track++;
            }
        }
    }

    avio_flush(pb);

    if (mov->flags & FF_MOV_FLAG_ISML)
        mov_write_isml_manifest(pb, mov, s);

    if (mov->flags & FF_MOV_FLAG_EMPTY_MOOV &&
        !(mov->flags & FF_MOV_FLAG_DELAY_MOOV)) {
        if ((ret = mov_write_moov_tag(pb, mov, s)) < 0)
            return ret;
        avio_flush(pb);
        mov->moov_written = 1;
        if (mov->flags & FF_MOV_FLAG_GLOBAL_SIDX)
            mov->reserved_header_pos = avio_tell(pb);
    }

    return 0;
}